// Exports Ghidra decompiler output for several functions in one headless run.
// Args: <timeout-seconds> <target>=<out-file> ...  (target: symbol name or 0x address)
// @category LSW1

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

import java.io.File;
import java.io.FileWriter;
import java.util.List;

public class ExportFunctionDecomp extends GhidraScript {
    private Function findFunction(String target) {
        if (target.startsWith("0x") || target.startsWith("0X")) {
            Address address = toAddr(Long.decode(target));
            Function function = getFunctionAt(address);
            return function != null ? function : getFunctionContaining(address);
        }
        List<Function> matches = getGlobalFunctions(target);
        return matches.isEmpty() ? null : matches.get(0);
    }

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("Usage: ExportFunctionDecomp.java <timeout> <target>=<out-file> ...");
            return;
        }
        int timeout = Integer.parseInt(args[0]);
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(currentProgram);
        decompiler.setOptions(options);
        decompiler.openProgram(currentProgram);
        try {
            for (int i = 1; i < args.length; i++) {
                int eq = args[i].indexOf('=');
                String target = args[i].substring(0, eq);
                File outFile = new File(args[i].substring(eq + 1));
                Function function = findFunction(target);
                if (function == null) {
                    println("error: function not found: " + target);
                    continue;
                }
                DecompileResults results = decompiler.decompileFunction(function, timeout, monitor);
                if (!results.decompileCompleted()) {
                    println("error: decompile failed for " + target + ": " + results.getErrorMessage());
                    continue;
                }
                outFile.getParentFile().mkdirs();
                try (FileWriter writer = new FileWriter(outFile)) {
                    writer.write("/* Ghidra reference output; control flow only, never matching source. */\n");
                    writer.write("/* " + function.getName() + " @ " + function.getEntryPoint()
                        + ", " + function.getBody().getNumAddresses() + " bytes */\n\n");
                    writer.write(results.getDecompiledFunction().getC());
                }
                println("wrote " + outFile.getPath());
            }
        } finally {
            decompiler.dispose();
        }
    }
}
