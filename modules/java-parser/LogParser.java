package org.syslog_ng;

public class LogParser extends LogPipe{
  public LogParser(long pipeHandle) {
 	  super(pipeHandle);
	}

	public void deinit() {
    System.out.println("java parser deinit");
	}

	public boolean init() {
		System.out.println("java parser init");
		return true;
	}
}
