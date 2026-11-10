// Applies paired GC names to the Japanese PC exe: functions (created where Ghidra has none)
// and data labels. Input lines: <hex x86 address> TAB <name> TAB function|label
// (written by tools/ghidra_decompile.py setup-pc / decompile-pc).
// @category LSW1

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

public class ApplyPcNames extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1 || !new File(args[0]).exists()) {
            printerr("Usage: ApplyPcNames.java /path/to/pc_names.tsv");
            return;
        }
        int functions = 0, created = 0, labels = 0, errors = 0;
        try (BufferedReader reader = new BufferedReader(new FileReader(args[0]))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] cols = line.split("\t");
                if (cols.length < 3 || cols[0].startsWith("#")) {
                    continue;
                }
                Address address = toAddr(Long.parseLong(cols[0].replaceFirst("^0x", ""), 16));
                String name = cols[1];
                try {
                    if ("function".equals(cols[2])) {
                        Function function = getFunctionAt(address);
                        if (function == null) {
                            disassemble(address);
                            function = createFunction(address, null);
                            if (function != null) {
                                created++;
                            }
                        }
                        if (function != null && !function.getName().equals(name)) {
                            function.setName(name, SourceType.USER_DEFINED);
                            functions++;
                        }
                    } else {
                        boolean present = false;
                        for (Symbol s : currentProgram.getSymbolTable().getSymbols(address)) {
                            present |= s.getName().equals(name);
                        }
                        if (!present) {
                            createLabel(address, name, true, SourceType.USER_DEFINED);
                            labels++;
                        }
                    }
                } catch (Exception e) {
                    errors++;
                    if (errors <= 10) {
                        println("warning: " + cols[0] + " " + name + ": " + e.getMessage());
                    }
                }
            }
        }
        println("applied " + functions + " function names (" + created + " functions created), "
            + labels + " labels, " + errors + " errors");
    }
}
