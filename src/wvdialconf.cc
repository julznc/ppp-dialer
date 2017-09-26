/*
 * Worldvisions Weaver Software:
 *   Copyright (C) 1997-2005 Net Integration Technologies, Inc.
 *
 * WvDial configuration utility.  Generates a basic wvdial.conf file.
 */

#include "uniconfroot.h"
#include "wvfile.h"
#include "wvmodemscan.h"
#include "wvstrutils.h"
#include "version.h"
#include <ctype.h>
#include <getopt.h>


void check_ppp_options()
{
    WvFile file("/etc/ppp/options", O_RDONLY);
    char *line;
    
    while ((line = file.getline()) != NULL)
    {
	line = trim_string(line);
	
	// comments and blank lines are ignored
	if (line[0] == '#'  ||  !line[0])
	    continue;
	
	// IP addresses are allowed
	if (strchr(line, '.') || strchr(line, ':'))
	    continue;
	
	// but baud rates and tty names are not!
	// a 'connect' line is usually bad too.
	if (isdigit(line[0])
	    || !strncmp(line, "/dev", 4)
	    || !strncmp(line, "tty",  3) 
	    || !strncmp(line, "cua",  3)
	    || !strncmp(line, "connect", 7))
	{
	    wvcon->print("\n*** WARNING!  Line \"%s\"\n"
		"   in /etc/ppp/options may conflict with wvdial!\n\n", line);
	}
    }
}

static void print_usage(const char *prog)
{
    const char *p = strrchr( prog, '/' );
    if ( p ) {
        prog = ++p;
    }

    printf("Usage: %s [-hv] [FILENAME]\n", prog);
    printf("Create or update a WvDial configuration file\n\n"
           "  -h --help       Give this help list\n"
           "  -v --version    Print program version\n\n"
           "You must specify the FILENAME of the "
           "configuration file to generate.\n");
}

static void parse_opts(int argc, char *argv[])
{
    int c;
    static const struct option lopts[] = {
        {"help",    no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
    };
    while (-1 != (c = getopt_long(argc, argv, "hv", lopts, NULL)))
    {
        switch (c)
        {
        case 'h':
            print_usage(argv[0]);
            exit(0);
            break;
        case 'v':
            printf("WvDialConf " WVDIAL_VER_STRING "\n"
                   "Copyright (c) 1997-2005 Net Integration "
                   "Technologies, Inc.\n"
                   "          (c) 2017 'yus\n");
            exit(0);
            break;
        default:
            print_usage(argv[0]);
            exit(-1);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    WvString conffilename("/etc/wvdial.conf");
    parse_opts(argc, argv);

    if (optind < argc) {
        //printf("FILENAME = %s", argv[optind]);
        conffilename = argv[optind];
    }

    wvcon->print("Editing `%s'.\n\n", conffilename);

    wvcon->print("Scanning your serial ports for a modem.\n\n");
    
    WvModemScanList l;
    while (!l.isdone())
	l.execute();
    
    if (l.count() < 1)
    {
	wvcon->print("\n\n"
	  "Sorry, no modem was detected!  "
	    "Is it in use by another program?\n"
	  "Did you configure it properly with setserial?\n\n"
		
	  "Please read the FAQ at http://alumnit.ca/wiki/?WvDial\n");
	return 1;
    }
    
    WvModemScanList::Iter i(l);
    
    i.rewind(); i.next();
    WvModemScan &m = *i;
    WvString fn = m.filename(), init = m.initstr();
    
    wvcon->print("\nFound %s on %s",
        m.is_isdn() ? "an ISDN TA" :
        strncmp("/dev/ttyACM",fn,11) ? "a modem" : "an USB modem", fn.cstr());
    if (m.use_modem_link) {
        wvcon->print(", using link /dev/modem in config.\n");
        fn = "/dev/modem";
    } else {
        wvcon->print(".\n");    
    }
    UniConfRoot root(WvString("ini:%s", conffilename), 0660);
    UniConf cfg(root["Dialer Defaults"]);
    cfg.xset("Modem", fn);
    cfg.xsetint("Baud", m.maxbaud());
    cfg.xset("Init1", (m.is_isdn() ? "AT&F" : "ATZ"));
    cfg.xset("Init2", init);
    cfg.xset("ISDN", (m.use_default_asyncmap() ? "1" : "0"));
    cfg.xset("Modem Name", (m.modem_name ? m.modem_name.cstr() : ""));
    cfg.xset("Modem Type", (m.is_isdn()
			    ? "ISDN Terminal Adapter"
			    : (strncmp("/dev/ttyACM",fn,11)
			       ? "Analog Modem" : "USB Modem")));  
 
    if (m.modem_name)
        wvcon->print("Config for %s written to %s.\n",
		     m.modem_name, conffilename);
    else
        wvcon->print("Modem configuration written to %s.\n", conffilename);

    // insert some entries to let people know what they need to edit
    if (!cfg.xget("Phone"))
	cfg.xset("; Phone", "<Target Phone Number>");
    if (!cfg.xget("Username"))
	cfg.xset("; Username", "<Your Login Name>");
    if (!cfg.xget("Password"))
	cfg.xset("; Password", "<Your Password>");
    
    check_ppp_options();
    
    cfg.commit();
    return 0;
}
