package fastjson;

/**
 * Exception thrown when JSON parsing fails.
 * 
 * @author FastJava Team
 * @version 1.0.0
 */
public class FastJsonParseException extends RuntimeException {
    
    private final int line;
    private final int column;
    private final String context;
    
    public FastJsonParseException(String message) {
        super(message);
        this.line = -1;
        this.column = -1;
        this.context = null;
    }
    
    public FastJsonParseException(String message, Throwable cause) {
        super(message, cause);
        this.line = -1;
        this.column = -1;
        this.context = null;
    }
    
    public FastJsonParseException(String message, int line, int column, String context) {
        super(message + " at line " + line + ", column " + column);
        this.line = line;
        this.column = column;
        this.context = context;
    }
    
    /**
     * Get the line number where the error occurred.
     * 
     * @return line number, or -1 if unknown
     */
    public int getLine() {
        return line;
    }
    
    /**
     * Get the column number where the error occurred.
     * 
     * @return column number, or -1 if unknown
     */
    public int getColumn() {
        return column;
    }
    
    /**
     * Get context around the error location.
     * 
     * @return context string, or null if unknown
     */
    public String getContext() {
        return context;
    }
}
