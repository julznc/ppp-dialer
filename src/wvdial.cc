/*
 * Worldvisions Weaver Software:
 *   Copyright (C) 1997-2005 Net Integration Technologies, Inc.
 *
 * Standalone WvDial program, for testing the WvDialer class.
 *
 * Created:	Sept 30 1997		D. Coombs
 */

#include "wvdialer.h"
#include "version.h"
#include "wvlog.h"
#include "wvlogrcv.h"
#include "wvsyslog.h"
#include "wvcrash.h"

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>

volatile bool want_to_die = false;


// use no prefix string for app "Modem", and an arrow for everything else.
// This makes the output of the wvdial application look nicer.
class WvDialLogger : public WvLogConsole
/**************************************/
{
public:
    WvDialLogger() : WvLogConsole(dup(2)) // log to stderr (fd 2)
        { }

protected:
    virtual void _make_prefix(time_t now);
};


void WvDialLogger::_make_prefix(time_t now)
/*******************************/
{
    WvString name = last_source;
    if(name == "WvDial Modem") 
    {
	prefix = "";
	prelen = 0;
    } 
    else 
    {
	prefix = "--> ";
	prelen = 4;
    }
}

static void signalhandler(int sig)
/***********************************/
{
    fprintf(stderr, "Caught signal %d:  Attempting to exit gracefully...\n", sig);
    want_to_die = true;
    signal(sig, SIG_DFL);
}


static bool haveconfig = false;
static bool chat_mode = false;
static bool write_syslog = true;
static WvConf *p_cfg = NULL;

static void print_usage(const char *prog)
{
    const char *p = strrchr( prog, '/' );
    if ( p ) {
        prog = ++p;
    }

    printf("Usage: %s [OPTION...] [SECTION]... [OPTION=value]...\n", prog);
    printf("An intelligent PPP dialer.\n\n"
           "  -c, --chat                 used when running wvdial from pppd\n"
           "  -C, --config=configfile    use configfile instead of /etc/wvdial.conf\n"
           "  -n, --no-syslog            don't send output to SYSLOG\n"
           "  -h, --help                 Give this help list\n"
           "  -v, --version              Print program version\n\n"
           "Optional SECTION arguments refer to sections in configuration file (usually)\n"
           "/etc/wvdial.conf, $HOME/.wvdialrc or the file specified by --config.\n"
           "Specified sections are all read, with later ones overriding previous ones.\n"
           "Any options not in the listed sections are taken from [Dialer Defaults].\n\n"
           "Also, optional OPTION=value parameters allow you to override options within\n"
           "the configuration files.");
}

static void parse_opts(int argc, char *argv[])
{
    int c;
    static const struct option lopts[] = {
        {"chat",      no_argument,       0, 'c'},
        {"config",    required_argument, 0, 'C'},
        {"no-syslog", no_argument,       0, 'n'},
        {"help",      no_argument,       0, 'h'},
        {"version",   no_argument,       0, 'v'},
    };
    while (-1 != (c = getopt_long(argc, argv, "cC:nhv", lopts, NULL)))
    {
        switch (c)
        {
        case 'c':
            chat_mode = true;
            break;
        case 'C':
            //printf("config=%s", optarg);
            if ((NULL!=p_cfg) && (!access(optarg, F_OK))) {
                p_cfg->load_file(optarg);
                haveconfig = true;
            } else {
                fprintf(stderr, "Cannot read '%s'\n", optarg);
                exit(-1);
            }
            break;
        case 'n':
            write_syslog = false;
            break;
        case 'h':
            print_usage(argv[0]);
            exit(0);
            break;
        case 'v':
            printf("WvDial " WVDIAL_VER_STRING "\n"
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
/********************************/
{
    WvSyslog        *syslog = NULL;
    WvStringList    sections;
    WvStringList    cmdlineopts;
    WvString        homedir = getenv("HOME");
    UniConfRoot     uniconf("temp:");
    WvConf          cfg(uniconf);

    signal(SIGTERM, signalhandler);
    signal(SIGINT, signalhandler);
    signal(SIGHUP, signalhandler);

    p_cfg = &cfg;
    parse_opts(argc, argv);
    while (optind < argc)
    {
        char *opt_arg = argv[optind];
        //printf("argv[%d]: %s\n", optind, opt_arg);
        if (strchr(opt_arg, '=')) {
            cmdlineopts.append(new WvString(opt_arg),true);
        } else {
            sections.append(new WvString("Dialer %s", opt_arg), true);
        }
        optind++;
    }

    if (sections.isempty())
    {
        sections.append(new WvString("Dialer Defaults"), true);
    }

    if( !haveconfig)
    {
        // Load the system file first...
        WvString stdconfig("/etc/wvdial.conf");

        if (!access(stdconfig, F_OK)) {
            cfg.load_file(stdconfig);
        }

        // Then the user specific one...
        if (homedir) {
            WvString rcfile("%s/.wvdialrc", homedir);

            if (!access(rcfile, F_OK))
            cfg.load_file(rcfile);
        }
    }
    
    // Inject all of the command line options on into the cfg file in a new
    // section called Command-Line if there are command line options.
    if (!cmdlineopts.isempty())
    {
        WvStringList::Iter i(cmdlineopts);
        for (i.rewind();i.next();) {
            char *name = i().edit();
            char *value = strchr(name,'=');

            // Value should never be null since it can't get into the list
            // if it doesn't have an = in i()
            // 
            *value = 0;
            value++;
            name = trim_string(name);
            value = trim_string(value);
            cfg.set("Command-Line", name, value);
        }
        sections.prepend(new WvString("Command-Line"), true);
    }
    
    if(!cfg.isok())
    {
        return 1;
    }
    
    if (chat_mode)
    {
        if (write_syslog) {
            WvString buf("wvdial[%s]", getpid());
            syslog = new WvSyslog( buf, false, WvLog::Debug2,
                                   WvLog::Debug2 );
        }
    }
    
    WvDialer dialer(cfg, &sections, chat_mode);
    
    if (!chat_mode)
    {
        if (dialer.isok() && dialer.options.ask_password)
            dialer.ask_password();
    }
    
    if (dialer.dial() == false)
    {
        return 1;
    }
    
    while (!want_to_die && dialer.isok()
           && dialer.status() != WvDialer::Idle)
    {
        dialer.select(100);
        dialer.callback();
    }
    
    int retval;
    
    if (want_to_die)
    {
        // Probably dieing from a user signal
        retval = 2;
    }
    
    if ((dialer.status() != WvDialer::Idle) || !dialer.isok()) {
        retval = 1;
    } else {
        retval = 0;
    }
    
    dialer.hangup();
    
    if (NULL!=syslog) {
        delete syslog;
    }

    return(retval);
}
