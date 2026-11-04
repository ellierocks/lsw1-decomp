#!/usr/bin/env bash
# Long unattended decomp-permuter queue that survives the launching session.
#
#   tools/permute_long.sh [--unit NAME] [--jobs N] [--total HOURS] [--rerun] [--dry-run] \
#       SLOTS HOURS_PER_TARGET [TARGET...]
#   tools/permute_long.sh [--unit NAME] [--rerun] --add TARGET...
#
# See --help. The queue is a backlog file plus a slot loop, so early matches free slots for
# the next target instead of leaving the machine idle.
set -u

DEFAULT_UNIT=lsw1-permute
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd) || exit 1

usage() {
    cat <<EOF
usage: tools/permute_long.sh [options] SLOTS HOURS_PER_TARGET [TARGET...]
       tools/permute_long.sh [options] --add TARGET...

Run a backlog of permuter jobs. TARGET... is written to work/permute/backlog.txt (deduped,
order kept; replaces leftovers; with no TARGET the existing backlog is continued). At most
SLOTS targets run at once; whenever a slot frees (a match under --stop-on-zero, a timeout,
a crash) the next backlog entry starts, until the backlog is empty and nothing runs, or
--total HOURS has passed. Each target gets min(HOURS_PER_TARGET, time left) hours
(fractions allowed). A missing work/permute/TARGET is made by 'python3
tools/permute_setup.py TARGET' first; failure logs 'skip TARGET setup-failed' and moves on. A
job is 'permute_setup.py TARGET --run -- -j JOBS --stop-on-zero --best-only' under nice.
Targets whose directory already holds an output-0* are skipped as matched unless --rerun.
One failing target never stops the queue.

The script re-executes itself as a transient systemd user unit wrapped in
'systemd-inhibit --what=sleep:idle --mode=block'. The unit is not in the cgroup scope of
the launching agent session or terminal, so it survives that scope ending (a setsid/nohup
run did not), and logind refuses the desktop idle policy's 'systemctl suspend' while the
inhibitor is held. Screen lock and screen-off still happen. The inhibitor is released when
the queue ends or the unit is stopped. Do not use setsid or nohup for long runs.

options:
  --unit NAME   systemd unit name (default $DEFAULT_UNIT); use another name to run a second
                queue next to the first. A non-default unit writes *-NAME files.
  --jobs N      permuter -j value per target (default 2)
  --total HOURS overall deadline: no launches after it, each timeout capped by the time left
                (default: run until the backlog drains)
  --rerun       do not skip targets that already have an output-0* (also for --add)
  --add         append TARGET... to the backlog of the running unit (skips targets that are
                queued, already ran this run or matched); exits 1 if the unit is not running
  --dry-run     print the systemd-run command and the queue, change nothing
  -h, --help    this text

control:
  systemctl --user stop UNIT        stop early; running jobs are stopped, the summary is
                                    written and the unstarted backlog is kept
  journalctl --user -u UNIT         launcher/systemd log
  python3 tools/permwatch.py        monitor (reads backlog and queue log)
  python3 tools/permute_review.py TARGET   review improved outputs
  python3 tools/permute_backlog.py  ranked candidates to fill a backlog

files (all under ignored work/permute/):
  backlog.txt         targets not started yet, one per line (pop and --add use backlog.lock)
  queue_targets.txt   targets of the latest launch
  queue.log           start/launch/finish/skip lines; 'finish NAME rc=N base=B best=S secs=T'
  queue_summary.txt   written when the queue ends or is stopped: matched, improved,
                      unchanged, failed and not started targets
  TARGET/overnight.log  permuter output; the previous run's is kept as overnight.prev.log
EOF
}

die() {
    echo "permute_long.sh: $*" >&2
    exit 2
}

unit=$DEFAULT_UNIT
njobs=2
dry=0
inner=0
add=0
rerun=0
total=""
pos=()
while [ $# -gt 0 ]; do
    case $1 in
        -h | --help) usage; exit 0 ;;
        --unit) [ $# -ge 2 ] || die "--unit needs a name"; unit=$2; shift 2 ;;
        --unit=*) unit=${1#--unit=}; shift ;;
        --jobs) [ $# -ge 2 ] || die "--jobs needs a number"; njobs=$2; shift 2 ;;
        --jobs=*) njobs=${1#--jobs=}; shift ;;
        --total) [ $# -ge 2 ] || die "--total needs hours"; total=$2; shift 2 ;;
        --total=*) total=${1#--total=}; shift ;;
        --rerun) rerun=1; shift ;;
        --add) add=1; shift ;;
        --dry-run) dry=1; shift ;;
        --inner) inner=1; shift ;; # internal: set by the re-exec below; tests run it directly
        --) shift; pos+=("$@"); break ;;
        -*) die "unknown option $1 (try --help)" ;;
        *) pos+=("$1"); shift ;; # options may also follow SLOTS HOURS
    esac
done
set -- "${pos[@]}"

slots=""
hours=""
if [ "$add" = 1 ]; then
    [ $# -ge 1 ] || { usage >&2; exit 2; }
    targets=("$@")
else
    [ $# -ge 2 ] || { usage >&2; exit 2; }
    slots=$1
    hours=$2
    shift 2
    targets=("$@")
fi

num_re='^[0-9]+(\.[0-9]+)?$'
[[ $unit =~ ^[A-Za-z0-9_.@-]+$ ]] || die "invalid unit name '$unit'"
[[ $njobs =~ ^[1-9][0-9]*$ ]] || die "--jobs must be a positive integer, got '$njobs'"
if [ "$add" = 0 ]; then
    [[ $slots =~ ^[1-9][0-9]*$ ]] || die "SLOTS must be a positive integer, got '$slots'"
    [[ $hours =~ $num_re ]] || die "HOURS_PER_TARGET must be a number, got '$hours'"
    awk -v h="$hours" 'BEGIN { exit !(h > 0) }' || die "HOURS_PER_TARGET must be positive"
fi
if [ -n "$total" ]; then
    [[ $total =~ $num_re ]] || die "--total must be a number of hours, got '$total'"
    awk -v h="$total" 'BEGIN { exit !(h > 0) }' || die "--total must be positive"
fi
for t in "${targets[@]}"; do
    [[ $t =~ ^[A-Za-z0-9_]+$ ]] || die "invalid target name '$t'"
done
# Dedupe, keep order.
mapfile -t targets < <(printf '%s\n' "${targets[@]}" | awk 'NF && !s[$0]++')

cd "$ROOT" || exit 1
pdir=work/permute
suffix=""
[ "$unit" = "$DEFAULT_UNIT" ] || suffix="-$unit"
queue_log=$pdir/queue$suffix.log
queue_targets=$pdir/queue_targets$suffix.txt
queue_summary=$pdir/queue_summary$suffix.txt
backlog=$pdir/backlog$suffix.txt
lock=$pdir/backlog$suffix.lock
pidfile=$pdir/queue$suffix.pid
# Test hooks: seconds per "hour" and the idle poll interval.
hour_secs=${PERMUTE_LONG_HOUR_SECS:-3600}
poll=${PERMUTE_LONG_POLL:-10}

fnow() { date +%s.%N; }

# Lines of the queue log from the latest 'start' line on.
log_since_start() {
    [ -f "$queue_log" ] || return 0
    awk '/^start /{n=0} {l[++n]=$0} END{for(i=1;i<=n;i++) print l[i]}' "$queue_log"
}

matched_dir() { compgen -G "$pdir/$1/output-0*" >/dev/null; }

queue_running() {
    local pid
    [ -f "$pidfile" ] || return 1
    pid=$(<"$pidfile")
    [[ $pid =~ ^[0-9]+$ ]] || return 1
    kill -0 "$pid" 2>/dev/null && grep -qa permute_long "/proc/$pid/cmdline" 2>/dev/null
}

if [ "$add" = 1 ]; then
    queue_running || {
        echo "permute_long.sh: unit $unit is not running; start it with" \
            "tools/permute_long.sh SLOTS HOURS TARGET..." >&2
        exit 1
    }
    command -v flock >/dev/null || die "flock not found"
    (
        flock 9
        touched=$(log_since_start | awk '$1=="launch"||$1=="finish"||$1=="skip"{print $2}')
        queued=$(cat "$backlog" 2>/dev/null)
        for t in "${targets[@]}"; do
            if grep -Fxq -- "$t" <<<"$touched"; then
                echo "skipped $t (running or already ran in this run)"
            elif grep -Fxq -- "$t" <<<"$queued"; then
                echo "skipped $t (already queued)"
            elif [ "$rerun" = 0 ] && matched_dir "$t"; then
                echo "skipped $t (already matched)"
            else
                echo "$t" >>"$backlog"
                queued+=$'\n'"$t"
                echo "added $t"
            fi
        done
    ) 9>"$lock"
    exit 0
fi

if [ "$inner" = 0 ]; then
    cmd=(systemd-run --user "--unit=$unit" --collect --same-dir "--setenv=PATH=$PATH")
    [ -z "${DECOMP_PERMUTER:-}" ] || cmd+=("--setenv=DECOMP_PERMUTER=$DECOMP_PERMUTER")
    cmd+=(systemd-inhibit --what=sleep:idle --mode=block "--who=$unit"
        "--why=Permuter queue ($unit)"
        bash "$ROOT/tools/permute_long.sh" --inner --unit "$unit" --jobs "$njobs")
    [ -z "$total" ] || cmd+=(--total "$total")
    [ "$rerun" = 0 ] || cmd+=(--rerun)
    cmd+=("$slots" "$hours")
    if [ "$dry" = 1 ]; then
        echo "dry run: nothing launched or written"
        printf 'systemd-run command:\n  '
        printf '%q ' "${cmd[@]}"
        echo
        echo "queue: unit=$unit slots=$slots hours=$hours total=${total:-none} jobs=$njobs" \
            "targets=${#targets[@]}"
        for t in "${targets[@]}"; do
            [ -d "$pdir/$t" ] || echo "  note: $pdir/$t missing; the queue runs permute_setup.py $t first"
            echo "  $t: timeout -k 60 min(${hours}h, time left) nice -n 10 python3 tools/permute_setup.py" \
                "$t --run -- -j $njobs --stop-on-zero --best-only >> $pdir/$t/overnight.log"
        done
        echo "files: $backlog $queue_targets $queue_log $queue_summary"
        exit 0
    fi
    command -v systemd-run >/dev/null || die "systemd-run not found"
    command -v systemd-inhibit >/dev/null || die "systemd-inhibit not found"
    command -v flock >/dev/null || die "flock not found"
    if systemctl --user is-active --quiet "$unit" || queue_running; then
        die "unit $unit is already running; add targets with --add, stop it (systemctl --user stop $unit) or use --unit NAME"
    fi
    if [ "${#targets[@]}" = 0 ]; then
        [ -s "$backlog" ] || die "no TARGET given and $backlog is empty"
        echo "continuing the existing backlog ($(grep -c . "$backlog") targets)"
    else
        mkdir -p "$pdir"
        if [ -s "$backlog" ]; then
            echo "note: replacing $(grep -c . "$backlog") leftover backlog entries in $backlog" >&2
        fi
        printf '%s\n' "${targets[@]}" >"$backlog"
        printf '%s\n' "${targets[@]}" >"$queue_targets"
        missing=0
        for t in "${targets[@]}"; do [ -d "$pdir/$t" ] || missing=$((missing + 1)); done
        [ "$missing" = 0 ] ||
            echo "warning: $missing of ${#targets[@]} targets have no $pdir/NAME yet;" \
                "the queue runs permute_setup.py for each when it starts" >&2
    fi
    exec "${cmd[@]}"
fi

# ---- Inner mode: running inside the transient unit (or directly, for tests) ----
mkdir -p "$pdir"
command -v flock >/dev/null || die "flock not found"
if [ "${#targets[@]}" -gt 0 ]; then
    ( flock 9; printf '%s\n' "${targets[@]}" >"$backlog" ) 9>"$lock"
fi
exec >>"$queue_log" 2>&1

tmpd=$(mktemp -d)
mkfifo "$tmpd/wake"
exec 8<>"$tmpd/wake"
cleanup() {
    rm -rf "$tmpd"
    if [ -f "$pidfile" ] && [ "$(<"$pidfile")" = "$$" ]; then rm -f "$pidfile"; fi
}
trap cleanup EXIT

backlog_count() { grep -c . "$backlog" 2>/dev/null || true; }

echo "start $(date -Is) unit=$unit slots=$slots per=${hours}h total=${total:-none} jobs=$njobs backlog=$(backlog_count)"
echo "$$" >"$pidfile"

deadline=""
if [ -n "$total" ]; then
    deadline=$(awk -v n="$(fnow)" -v t="$total" -v s="$hour_secs" 'BEGIN { printf "%.3f", n + t * s }')
fi
min_launch=$(awk -v s="$hour_secs" 'BEGIN { printf "%.3f", s / 60 }')

secs_left() { awk -v d="$deadline" -v n="$(fnow)" 'BEGIN { printf "%.3f", d - n }'; }

# Print the next backlog entry and remove it; status 1 when empty.
pop_next() {
    (
        flock 9
        t=$(grep -m1 -v '^[[:space:]]*$' "$backlog" 2>/dev/null) || exit 1
        awk -v t="$t" 'NF && (d || $0 != t) { print; next } NF { d = 1 }' "$backlog" >"$backlog.tmp" &&
            mv "$backlog.tmp" "$backlog"
        printf '%s\n' "$t"
    ) 9>"$lock"
}

requeue() {
    ( flock 9; { echo "$1"; cat "$backlog" 2>/dev/null; } >"$backlog.tmp" && mv "$backlog.tmp" "$backlog" ) 9>"$lock"
}

# Lowest output-SCORE-* score for target $1, or empty.
best_of() {
    local d s best=""
    for d in "$pdir/$1"/output-*; do
        [ -d "$d" ] || continue
        s=${d##*/output-}
        s=${s%%-*}
        [[ $s =~ ^[0-9]+$ ]] || continue
        if [ -z "$best" ] || [ "$s" -lt "$best" ]; then best=$s; fi
    done
    echo "$best"
}

base_of() {
    local b
    b=$(grep -a -m1 -o 'base score = [0-9]*' "$pdir/$1/overnight.log" 2>/dev/null) && { echo "${b##* }"; return; }
    b=$(grep -a -m1 -o '^base: [0-9]*' "$pdir/$1/overnight.log" 2>/dev/null) && echo "${b##* }"
}

# Run "${@:2}" with stdout/stderr appended to $1 and stop it on SIGTERM; sets rc.
supervise() {
    local log=$1
    shift
    rc=143
    [ "$term" = 0 ] || return 0
    "$@" >>"$log" 2>&1 8>&- &
    cpid=$!
    wait "$cpid"
    rc=$?
    while [ "$term" = 1 ] && kill -0 "$cpid" 2>/dev/null; do
        wait "$cpid"
        rc=$?
    done
    cpid=""
}

# One slot's work, run in a background subshell: setup if needed, the job, the finish line.
run_target() {
    local t=$1 limit=$2 started out="" base best secs jobcmd
    term=0
    cpid=""
    rc=0
    trap 'term=1; [ -z "$cpid" ] || kill -TERM "$cpid" 2>/dev/null' TERM
    started=$(fnow)
    if [ ! -d "$pdir/$t" ]; then
        out=$(mktemp)
        supervise "$out" nice -n 10 python3 tools/permute_setup.py "$t"
        if [ "$term" = 1 ]; then
            requeue "$t"
            echo "requeue $t"
            rm -f "$out"
            echo "$t" >&8
            return
        fi
        if [ "$rc" != 0 ]; then
            echo "skip $t setup-failed"
            tail -n 3 "$out" | sed 's/^/  setup: /'
            rm -f "$out"
            echo "$t" >&8
            return
        fi
    fi
    [ ! -e "$pdir/$t/overnight.log" ] || mv -f "$pdir/$t/overnight.log" "$pdir/$t/overnight.prev.log"
    if [ -n "$out" ]; then
        cat "$out" >>"$pdir/$t/overnight.log"
        rm -f "$out"
    fi
    if [ -n "${PERMUTE_LONG_JOB_CMD:-}" ]; then
        read -r -a jobcmd <<<"$PERMUTE_LONG_JOB_CMD"
        jobcmd+=("$t" "$njobs")
    else
        jobcmd=(python3 tools/permute_setup.py "$t" --run -- -j "$njobs" --stop-on-zero --best-only)
    fi
    supervise "$pdir/$t/overnight.log" timeout -k 60 "${limit}s" nice -n 10 "${jobcmd[@]}"
    secs=$(awk -v a="$started" -v b="$(fnow)" 'BEGIN { printf "%d", b - a + 0.5 }')
    base=$(base_of "$t")
    best=$(best_of "$t")
    echo "finish $t rc=$rc base=${base:-?} best=${best:-none} secs=$secs"
    echo "$t" >&8
}

write_summary() {
    {
        echo "$1 $(date -Is) unit=$unit"
        log_since_start | awk '
            function num(s) { return s ~ /^[0-9]+$/ }
            $1 == "start" { next }
            $1 == "launch" { if (!($2 in ev)) order[++n] = $2; ev[$2] = "running"; next }
            $1 == "requeue" { ev[$2] = "requeue"; next }
            $1 == "skip" { if (!($2 in ev)) order[++n] = $2; ev[$2] = "skip " $3; next }
            $1 == "finish" {
                ev[$2] = "finish"
                for (i = 3; i <= NF; i++) { split($i, kv, "="); v[$2, kv[1]] = kv[2] }
                next
            }
            END {
                for (i = 1; i <= n; i++) {
                    t = order[i]; e = ev[t]
                    rc = v[t, "rc"]; base = v[t, "base"]; best = v[t, "best"]
                    detail = base "→" best "  rc=" rc "  secs=" v[t, "secs"]
                    if (e == "requeue") continue
                    if (e == "skip matched") { m[++nm] = t "  (already matched)" }
                    else if (e ~ /^skip/) { f[++nf] = t "  " substr(e, 6) }
                    else if (e == "running") { x[++nx] = t "  (interrupted)" }
                    else if (best == "0") { m[++nm] = t "  " detail }
                    else if (num(best) && num(base) && best + 0 < base + 0) { im[++ni] = t "  " detail }
                    else if (rc != "0" && rc != "124" && rc != "143") { f[++nf] = t "  " detail }
                    else { u[++nu] = t "  " detail }
                }
                sect("matched", m, nm); sect("improved", im, ni); sect("unchanged", u, nu)
                sect("failed", f, nf); if (nx) sect("interrupted", x, nx)
            }
            function sect(name, arr, cnt,   i) {
                print name " (" cnt + 0 "):"
                for (i = 1; i <= cnt; i++) print "  " arr[i]
            }'
        local left
        left=$(grep -v '^[[:space:]]*$' "$backlog" 2>/dev/null || true)
        echo "not started ($(grep -c . <<<"$left" || true)):"
        [ -z "$left" ] || awk '{ print "  " $0 }' <<<"$left"
    } >"$queue_summary.tmp"
    mv "$queue_summary.tmp" "$queue_summary"
    cat "$queue_summary"
}

stopping=0
active=0
pids=()
declare -A seen=()

on_term() {
    stopping=1
    local p
    for p in "${pids[@]}"; do kill -TERM "$p" 2>/dev/null; done
}
trap on_term TERM INT

while [ "$stopping" = 0 ]; do
    left=""
    past=0
    if [ -n "$deadline" ]; then
        left=$(secs_left)
        awk -v l="$left" -v m="$min_launch" 'BEGIN { exit !(l < m) }' && past=1
    fi
    if [ "$active" -lt "$slots" ] && [ "$past" = 0 ]; then
        if t=$(pop_next); then
            if [[ ! $t =~ ^[A-Za-z0-9_]+$ ]]; then
                echo "skip $t invalid-name"
            elif [ -n "${seen[$t]:-}" ]; then
                :
            elif [ "$rerun" = 0 ] && matched_dir "$t"; then
                seen[$t]=1
                echo "skip $t matched"
            else
                seen[$t]=1
                limit=$(awk -v h="$hours" -v s="$hour_secs" -v l="$left" \
                    'BEGIN { t = h * s; if (l != "" && l + 0 < t) t = l + 0; printf "%.3f", t }')
                echo "launch $t $(date -Is) timeout=${limit}s"
                run_target "$t" "$limit" &
                pids+=($!)
                active=$((active + 1))
            fi
            continue
        fi
    fi
    [ "$active" -gt 0 ] || break
    if read -r -t "$poll" -u 8 _; then active=$((active - 1)); fi
done

reason="done"
[ "$stopping" = 0 ] || reason="stopped (signal)"
wait
write_summary "$reason"
