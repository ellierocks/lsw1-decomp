// Exports Ghidra decompiler output for one function by name or address.
// @category LSW1

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

import java.io.File;
import java.io.FileWriter;

public class ExportFunctionDecomp extends GhidraScript {
    private Function findFunction(String target) throws Exception {
        if (target.startsWith("0x") || target.startsWith("0X")) {
            long offset = Long.decode(target);
            Address address = currentProgram.getAddressFactory()
                .getDefaultAddressSpace()
                .getAddress(offset);
            Function function = getFunctionAt(address);
            if (function == null) {
                function = getFunctionContaining(address);
            }
            return function;
        }

        FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            Function function = functions.next();
            if (function.getName().equals(target)) {
                return function;
            }
        }
        return null;
    }

    private String safeFileName(String name) {
        return name.replaceAll("[^A-Za-z0-9_.-]", "_");
    }

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("Usage: ExportFunctionDecomp.java <function-or-addr> <out-dir> [timeout-seconds]");
            return;
        }

        String target = args[0];
        File outDir = new File(args[1]);
        int timeout = args.length >= 3 ? Integer.parseInt(args[2]) : 60;
        outDir.mkdirs();

        Function function = findFunction(target);
        if (function == null) {
            printerr("Function not found: " + target);
            return;
        }

        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        DecompileResults results = decompiler.decompileFunction(function, timeout, monitor);
        if (!results.decompileCompleted()) {
            printerr("Decompile failed for " + function.getName() + ": " + results.getErrorMessage());
            return;
        }

        File outFile = new File(outDir, safeFileName(function.getName()) + ".c");
        try (FileWriter writer = new FileWriter(outFile)) {
            writer.write("/* Ghidra decompiler reference output. Do not commit as matching source. */\n");
            writer.write("/* Function: " + function.getName() + " @ " + function.getEntryPoint() + " */\n\n");
            writer.write(results.getDecompiledFunction().getC());
            writer.write("\n");
        }

        println("Wrote " + outFile.getAbsolutePath());
    }
}
