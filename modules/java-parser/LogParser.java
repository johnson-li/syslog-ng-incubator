package org.syslog_ng;

public class LogParser extends LogPipe{
  public LogParser(long pipeHandle) {
 	  super(pipeHandle);
		System.out.println("LogPipe constructor");
	}

	public void deinit() {
    System.out.println("java parser deinit");
	}

	public boolean init() {
		System.out.println("java parser init");
		return true;
	}

	public boolean process(String msg) {
    System.out.println("processing...");
		return true;
	}
}
