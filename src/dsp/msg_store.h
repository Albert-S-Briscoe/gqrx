
#ifndef MSG_STORE_H
#define MSG_STORE_H

#include <mutex>
#include <gnuradio/block.h>
#include <queue>

class msg_store;

#if GNURADIO_VERSION < 0x030900
typedef boost::shared_ptr<msg_store> msg_store_sptr;
#else
typedef std::shared_ptr<msg_store> msg_store_sptr;
#endif

msg_store_sptr make_msg_store();

class msg_store : public gr::block
{
public:
    msg_store();
    ~msg_store();

    void get_message(std::vector<std::string> &out, int &type);

private:
    void store(pmt::pmt_t msg);

protected:
    std::mutex d_mutex;
    std::queue<pmt::pmt_t> d_messages;
};

#endif
