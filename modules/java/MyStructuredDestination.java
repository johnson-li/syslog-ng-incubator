/*
 * Copyright (c) 2014 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2014 Viktor Juhasz <viktor.juhasz@balabit.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */
package org.syslog_ng;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MyStructuredDestination extends StructuredLogDestination {

  private String name;

  public MyStructuredDestination(long arg0) {
    super(arg0);
  }

  public void deinit() {
    System.out.println("Deinit");
  }

  public void onMessageQueueEmpty() {
    System.out.println("onMessageQueueEmpty");
    return;
  }

  public boolean init() {
    name = getOption("name");
    if (name == null) {
      System.err.println("Name is a required option for this destination");
      return false;
    }
    System.out.println("Init " + name);
    return true;
  }

  public boolean open() {
    return true;
  }

  public boolean isOpened() {
    return true;
  }

  public void close() {
    System.out.println("close");
  }

  public boolean send(LogMessage arg0) {
    MyLogMessage myLogMessage = new MyLogMessage(arg0);
    String message = myLogMessage.getValue("MESSAGE"); 
    
    //get and set key value pair
    Pattern pattern = Pattern.compile(".*\\{(\\w+)[ \\t]*:[ \\t]*(\\w+)\\}.*");    
    Matcher matcher = pattern.matcher(message);
    if (!matcher.matches()) {
      System.err.println("error parsing message: " + message);
      return false;
    }
    myLogMessage.setValue(matcher.group(1), matcher.group(2));

    myLogMessage.printAll();
    //arg0.release();
    return true;
  }
}
