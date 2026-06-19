// Applies decomp-toolkit symbols.txt names to the current Ghidra program.
// @category LSW1

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ApplySymbolsTxt extends GhidraScript {
    private static final Pattern SYMBOL_LINE = Pattern.compile(
        "^([A-Za-z_.$][A-Za-z0-9_.$]*)\\s*=\\s*\\.[A-Za-z0-9_]+:0x([0-9A-Fa-f]{8});.*type:([A-Za-z]+).*"
    );

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            printerr("Usage: ApplySymbolsTxt.java /path/to/symbols.txt");
            return;
        }

        File symbols = new File(args[0]);
        if (!symbols.exists()) {
            printerr("symbols.txt not found: " + symbols);
            return;
        }

        int functionCount = 0;
        int labelCount = 0;
        int errorCount = 0;

        try (BufferedReader reader = new BufferedReader(new FileReader(symbols))) {
            String line;
            while ((line = reader.readLine()) != null) {
                Matcher matcher = SYMBOL_LINE.matcher(line.trim());
                if (!matcher.matches()) {
                    continue;
                }

                String name = matcher.group(1);
                long offset = Long.parseUnsignedLong(matcher.group(2), 16);
                String type = matcher.group(3);
                Address address = currentProgram.getAddressFactory()
                    .getDefaultAddressSpace()
                    .getAddress(offset);

                try {
                    if ("function".equals(type)) {
                        Function function = getFunctionAt(address);
                        if (function == null) {
                            disassemble(address);
                            function = createFunction(address, name);
                        }
                        if (function != null) {
                            function.setName(name, SourceType.USER_DEFINED);
                            functionCount++;
                        }
                    } else {
                        createLabel(address, name, true, SourceType.USER_DEFINED);
                        labelCount++;
                    }
                } catch (Exception ex) {
                    errorCount++;
                    println("Failed: " + name + " @ " + address + " (" + ex.getMessage() + ")");
                }
            }
        }

        println("Applied symbols: functions=" + functionCount + ", labels=" + labelCount + ", errors=" + errorCount);
    }
}
