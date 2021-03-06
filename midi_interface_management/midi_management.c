#include "midi_management.h"

static snd_seq_t *seq;
static int port_count = 1;
static snd_seq_addr_t *ports;



/* prints an error message to stderr, and dies */
void fatal(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

/* memory allocation error handling */
void check_mem(void *p)
{
    if (!p)
        fatal("Out of memory");
}

/* error handling for ALSA functions */
void check_snd(const char *operation, int err)
{
    if (err < 0)
        fatal("Cannot %s - %s", operation, snd_strerror(err));
}

// Classic sequencer initialisation (ALSA lib)
void init_seq(void)
{
    int err;

    /* open sequencer */
    err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX | SND_SEQ_NONBLOCK, 0);
    check_snd("open sequencer", err);

    /* set our client's name */
    err = snd_seq_set_client_name(seq, "Midi_client");
    check_snd("set client name", err);
}

void connect_to_port(char *port_name)
{
    ports = realloc(ports, port_count * sizeof(snd_seq_addr_t));
    int err = snd_seq_parse_address(seq, &ports[0], port_name);
    if (err < 0)
    {
        fatal("Invalid port %s - %s", port_name, snd_strerror(err));
    }   
}

/* parses one or more port addresses from the string */
void parse_ports(const char *arg)
{
    char *buf, *s, *port_name;

    /* make a copy of the string because we're going to modify it */
    buf = strdup(arg);
    check_mem(buf);

    for (port_name = s = buf; s; port_name = s + 1)
    {
        /* Assume that ports are separated by commas.  We don't use
         spaces because those are valid in client names. */
        printf("%s\n", port_name);
        s = strchr(port_name, ',');
        if (s)
            *s = '\0';

        ++port_count;
        connect_to_port(port_name);
    }
    free(buf);
}



snd_seq_t *create_port(void)
{
    int err;

    err = snd_seq_create_simple_port(seq, "My_client",
                                     SND_SEQ_PORT_CAP_WRITE |
                                         SND_SEQ_PORT_CAP_SUBS_WRITE,
                                     SND_SEQ_PORT_TYPE_MIDI_GENERIC |
                                         SND_SEQ_PORT_TYPE_APPLICATION);
    check_snd("create port", err);
    return seq;
}

int connect_ports(snd_seq_t *seq1)
{
    int i, err;

    for (i = 0; i < port_count; ++i)
    {
        err = snd_seq_connect_from(seq1, 0, ports[i].client, ports[i].port);
        if (err < 0)
            fatal("Cannot connect from port %d:%d - %s",
                  ports[i].client, ports[i].port, snd_strerror(err));
    }

    return port_count;
}

void dump_event(const snd_seq_event_t *ev, ud *data)
{
    switch (ev->type)
    {
    case SND_SEQ_EVENT_NOTEON:
        if (ev->data.note.velocity)
        {
            data->all_keys->keys[(int)ev->data.note.note] = 1;
            key_on(data, (int)ev->data.note.note);
            init_rect_from_key((int)ev->data.note.note, data);
            printf("On %d\n", (int)ev->data.note.note);
        }
        else
        {
            data->all_keys->keys[(int)ev->data.note.note] = 0;
            key_off(data, (int)ev->data.note.note);
            release_rect_from_key((int)ev->data.note.note, data);
            printf("Off %d\n", (int)ev->data.note.note);
        }
        break;
    case SND_SEQ_EVENT_NOTEOFF:
        data->all_keys->keys[(int)ev->data.note.note] = 0;
        key_off(data, (int)ev->data.note.note);
        release_rect_from_key((int)ev->data.note.note, data);
        printf("Off %d\n", (int)ev->data.note.note);
        break;
    }
}

void list_ports(void)
{
    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t *pinfo;
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);
    while (snd_seq_query_next_client(seq, cinfo) >= 0)
    {
        int client = snd_seq_client_info_get_client(cinfo);

        snd_seq_port_info_set_client(pinfo, client);
        snd_seq_port_info_set_port(pinfo, -1);
        while (snd_seq_query_next_port(seq, pinfo) >= 0)
        {
            /* we need both READ and SUBS_READ */
            if ((snd_seq_port_info_get_capability(pinfo) & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ)) != (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
                continue;

            printf("%3d:%-32.32s\n",
                    snd_seq_port_info_get_client(pinfo),
                    snd_seq_client_info_get_name(cinfo));
        }
    }
}

int parse_input(int argc, char *argv[])
{
    int c;
    static const char short_options[] = "hVlp:";
    static const struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {"version", 0, NULL, 'V'},
        {"list", 0, NULL, 'l'},
        {"port", 1, NULL, 'p'},
        {}};
    while ((c = getopt_long(argc, argv, short_options,
                            long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'h':
            return 0;
        case 'V':

            return 0;
        case 'l':
            list_ports();
            return 0;
            break;
        case 'p':
            connect_to_port("20");
            break;
        default:
            return 1;
        }
    }
    return -1;
}