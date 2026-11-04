#!/usr/bin/env bash
# Long unattended decomp-permuter queue that survives the launching session.
#
#   tools/permute_long.sh [--unit NAME] [--jobs N] [--dry-run] SLOTS HOURS_PER_TARGET TARGET...
#
# Run `python3 tools/permute_setup.py TARGET` first for every TARGET. See --help.
set -u

DEFAULT_UNIT=lsw1-permute
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd) || exit 1

usage() {
    cat <<EOF
usage: tools/permute_long.sh [options] SLOTS HOURS_PER_TARGET TARGET...

Run a slot-limited queue of permuter jobs for TARGET... (each needs work/permute/TARGET,
made by 'python3 tools/permute_setup.py TARGET'). At most SLOTS targets run at once and
each gets HOURS_PER_TARGET hours (fractions allowed) before 'timeout' stops it. A job is
'permute_setup.py TARGET --run -- -j JOBS --stop-on-zero --best-only' under nice.

The script re-executes itself as a transient systemd user unit wrapped in
'systemd-inhibit --what=sleep:idle --mode=block'. The unit is not in the cgroup scope of
the launching agent session or terminal, so it survives that scope ending (a setsid/nohup
run did not), and logind refuses the desktop idle policy's 'systemctl suspend' while the
inhibitor is held. Screen lock and screen-off still happen. The inhibitor is released when
the queue ends or the unit is stopped. Do not use setsid or nohup for long runs.

options:
  --unit NAME   systemd unit name (default $DEFAULT_UNIT); use another name to run a second
                queue next to the first. A non-default unit writes queue*-NAME files.
  --jobs N      permuter -j value per target (default 2)
  --dry-run     print the systemd-run command and the queue, change nothing
  -h, --help    this text

control:
  systemctl --user stop UNIT        stop the whole queue early
  journalctl --user -u UNIT         launcher/systemd log
  python3 tools/permwatch.py        monitor (reads the recorded queue)
  python3 tools/permute_review.py TARGET   review improved outputs

files (all under ignored work/permute/):
  queue_targets.txt   targets of the latest launch, one per line (permwatch reads it)
  queue.log           launcher log (start, launch, done lines)
  queue_summary.txt   written when the queue ends: base score and best output per target
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
while [ $# -gt 0 ]; do
    case $1 in
        -h | --help) usage; exit 0 ;;
        --unit) [ $# -ge 2 ] || die "--unit needs a name"; unit=$2; shift 2 ;;
        --unit=*) unit=${1#--unit=}; shift ;;
        --jobs) [ $# -ge 2 ] || die "--jobs needs a number"; njobs=$2; shift 2 ;;
        --jobs=*) njobs=${1#--jobs=}; shift ;;
        --dry-run) dry=1; shift ;;
        --inner) inner=1; shift ;; # internal: set by the re-exec below
        --) shift; break ;;
        -*) die "unknown option $1 (try --help)" ;;
        *) break ;;
    esac
done

[ $# -ge 3 ] || { usage >&2; exit 2; }
slots=$1
hours=$2
shift 2
targets=("$@")

[[ $unit =~ ^[A-Za-z0-9_.@-]+$ ]] || die "invalid unit name '$unit'"
[[ $slots =~ ^[1-9][0-9]*$ ]] || die "SLOTS must be a positive integer, got '$slots'"
[[ $njobs =~ ^[1-9][0-9]*$ ]] || die "--jobs must be a positive integer, got '$njobs'"
[[ $hours =~ ^[0-9]+(\.[0-9]+)?$ ]] || die "HOURS_PER_TARGET must be a number, got '$hours'"

cd "$ROOT" || exit 1
pdir=work/permute
suffix=""
[ "$unit" = "$DEFAULT_UNIT" ] || suffix="-$unit"
queue_log=$pdir/queue$suffix.log
queue_targets=$pdir/queue_targets$suffix.txt
queue_summary=$pdir/queue_summary$suffix.txt

for t in "${targets[@]}"; do
    [[ $t =~ ^[A-Za-z0-9_]+$ ]] || die "invalid target name '$t'"
    [ -d "$pdir/$t" ] ||
        die "no $pdir/$t directory; create it with: python3 tools/permute_setup.py $t"
done

if [ "$inner" = 0 ]; then
    cmd=(systemd-run --user "--unit=$unit" --collect --same-dir "--setenv=PATH=$PATH")
    [ -z "${DECOMP_PERMUTER:-}" ] || cmd+=("--setenv=DECOMP_PERMUTER=$DECOMP_PERMUTER")
    cmd+=(systemd-inhibit --what=sleep:idle --mode=block "--who=$unit"
        "--why=Permuter queue ($unit)"
        bash "$ROOT/tools/permute_long.sh" --inner --unit "$unit" --jobs "$njobs"
        "$slots" "$hours" "${targets[@]}")
    if [ "$dry" = 1 ]; then
        echo "dry run: nothing launched or written"
        printf 'systemd-run command:\n  '
        printf '%q ' "${cmd[@]}"
        echo
        echo "queue: unit=$unit slots=$slots hours=$hours jobs=$njobs"
        for t in "${targets[@]}"; do
            echo "  $t: timeout -k 60 ${hours}h nice -n 10 python3 tools/permute_setup.py $t --run" \
                "-- -j $njobs --stop-on-zero --best-only >> $pdir/$t/overnight.log"
        done
        echo "files: $queue_targets $queue_log $queue_summary"
        exit 0
    fi
    command -v systemd-run >/dev/null || die "systemd-run not found"
    command -v systemd-inhibit >/dev/null || die "systemd-inhibit not found"
    if systemctl --user is-active --quiet "$unit"; then
        die "unit $unit is already running; stop it (systemctl --user stop $unit) or use --unit NAME"
    fi
    printf '%s\n' "${targets[@]}" >"$queue_targets"
    # Keep the previous run's log so a stale log does not read as this run's progress.
    for t in "${targets[@]}"; do
        [ ! -e "$pdir/$t/overnight.log" ] || mv -f "$pdir/$t/overnight.log" "$pdir/$t/overnight.prev.log"
    done
    exec "${cmd[@]}"
fi

# Inner mode: running inside the transient unit, holding the inhibitor.
exec >>"$queue_log" 2>&1
echo "start $(date -Is) unit=$unit slots=$slots per=${hours}h jobs=$njobs targets=${targets[*]}"
for t in "${targets[@]}"; do
    while [ "$(jobs -rp | wc -l)" -ge "$slots" ]; do wait -n; done
    echo "launch $t $(date -Is)"
    timeout -k 60 "${hours}h" nice -n 10 python3 tools/permute_setup.py "$t" --run \
        -- -j "$njobs" --stop-on-zero --best-only >>"$pdir/$t/overnight.log" 2>&1 &
done
wait
{
    echo "done $(date -Is)"
    for t in "${targets[@]}"; do
        best=""
        best_score=""
        for d in "$pdir/$t"/output-*; do
            [ -d "$d" ] || continue
            s=${d##*/output-}
            s=${s%%-*}
            if [ -z "$best_score" ] || [ "$s" -lt "$best_score" ]; then
                best_score=$s
                best=${d##*/}
            fi
        done
        base=$(grep -a -m1 -o 'base score = [0-9]*' "$pdir/$t/overnight.log" 2>/dev/null)
        echo "$t: ${base:-base score unknown} best=${best:-none}"
    done
} >"$queue_summary"
cat "$queue_summary"
