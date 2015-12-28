#include <engine/dimmer.h>

#include <logger.h>
#include <engine/exceptions.h>

#include <sstream>

#include <engine/channel.h>

/*
 * Global stuff
 */
unsigned int Dimmer::num_dimmers = 0;
std::vector<Dimmer *> Dimmer::all_dimmers;



Dimmer::Dimmer(Universe & universe, const unsigned int dimmer) :
                   m_universe(universe), m_dimmer(dimmer)
{
    // Generate <universe>.<dimmer> as string. Is this ugly or what?
    // Also note that the name string is 1-based while our internal numbering
    // is 0-based.
    std::stringstream ss;
    ss << m_universe.getInternalUniverse() << "." << (m_dimmer + 1);
    m_name_string=ss.str();

    m_level = 0;
    m_universe.setLevel(m_dimmer, m_level);

    m_p_patchedChannel = NULL;
}


void Dimmer::patchToChannel(Channel * p_channel)
{
    if (m_p_patchedChannel)
    {
        LOG_ERROR("Error patching channel " << (p_channel->getNum() + 1) << " to dimmer " << m_name_string << \
                  ". Dimmer already patched.");
        throw AlreadyPatchedException();
    }
    m_p_patchedChannel = p_channel;
    setLevel(m_p_patchedChannel->getOutputLevel());
}


void Dimmer::unpatchFromChannel(Channel * p_channel)
{
    if (m_p_patchedChannel != p_channel)
    {
        LOG_ERROR("Error unpatching channel " << (p_channel->getNum() + 1) << " from dimmer " << m_name_string << \
                  ". Dimmer not patched to this channel.");
        throw NotPatchedException();
    }
    m_p_patchedChannel = NULL;
    setLevel(0);
}


void Dimmer::setLevel(const unsigned int level)
{
    // TODO: - Apply Dimmer Curve
    //       - Enforce Min and Max levels

    m_level = level;
    m_universe.setLevel(m_dimmer, m_level);
}

void Dimmer::createDimmers(const unsigned int num_dimmers, Universe & universe)
{
    all_dimmers.reserve(Dimmer::num_dimmers + num_dimmers);
    for (unsigned int i = 0; i < num_dimmers; i++) {
        all_dimmers.push_back(new Dimmer(universe, Dimmer::num_dimmers++));
    }
}

