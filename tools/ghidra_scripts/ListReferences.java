// Prints references to symbols or addresses: "xref <target> <from> <type> <function>".
// Args: <target> ...  (symbol name or 0x address)
// @category LSW1

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.Symbol;

public class ListReferences extends GhidraScript {
    @Override
    protected void run() throws Exception {
        for (String target : getScriptArgs()) {
            Address address = null;
            if (target.startsWith("0x") || target.startsWith("0X")) {
                address = toAddr(Long.decode(target));
            } else {
                for (Symbol symbol : getSymbols(target, null)) {
                    address = symbol.getAddress();
                    break;
                }
            }
            if (address == null) {
                println("error: symbol not found: " + target);
                continue;
            }
            int count = 0;
            for (Reference ref : getReferencesTo(address)) {
                if (!ref.getFromAddress().isMemoryAddress()) {
                    continue;  // synthetic "Entry Point" reference from the symbol table
                }
                Function function = getFunctionContaining(ref.getFromAddress());
                println("xref " + target + " " + ref.getFromAddress() + " " + ref.getReferenceType()
                    + " " + (function == null ? "-" : function.getName()));
                count++;
            }
            println("total " + target + " @ " + address + ": " + count);
        }
    }
}
