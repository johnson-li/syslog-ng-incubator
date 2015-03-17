package org.syslog_ng;

/**
 * Created by johnson on 3/15/15.
 */
public class MyLogMessage extends LogMessage{
    public MyLogMessage(long handle) {
        super(handle);
    }

    public MyLogMessage(LogMessage logMessage) {
        super(logMessage.getHandle());
    }

    @Override
    protected long getHandle() {
        return super.getHandle();
    }

    @Override
    public void release() {
        System.out.println("release");
        super.release();
    }

    @Override
    public String getValue(String s) {
        return super.getValue(s);
    }

    public void setValue(String key, String value) {
        setValue(getHandle(), key, value);
    } 

    public void printAll() {
        printAll(getHandle());
    }

    public native void setValue(long ptr, String key, String value);

    public native void printAll(long ptr);
}
