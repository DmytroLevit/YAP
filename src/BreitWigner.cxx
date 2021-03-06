#include "BreitWigner.h"

#include "FourMomenta.h"
#include "logging.h"
#include "Model.h"
#include "Resonance.h"

namespace yap {

//-------------------------
BreitWigner::BreitWigner(double w) :
    MassShapeWithNominalMass(),
    Width_(std::make_shared<RealParameter>(w))
{
    T()->addDependency(width());
}

//-------------------------
void BreitWigner::setParameters(const ParticleTableEntry& entry)
{
    MassShapeWithNominalMass::setParameters(entry);

    if (entry.MassShapeParameters.empty())
        throw exceptions::Exception("entry.MassShapeParameter is empty", "BreitWigner::setParameters");

    if (width()->value() < 0)
        width()->setValue(entry.MassShapeParameters[0]);
}

//-------------------------
std::complex<double> BreitWigner::amplitude(DataPoint& d, const std::shared_ptr<ParticleCombination>& pc, StatusManager& sm) const
{
    unsigned symIndex = symmetrizationIndex(pc);

    // recalculate, cache, & return, if necessary
    if (sm.status(*T(), symIndex) == kUncalculated) {

        // T = 1 / (M^2 - m^2 - iMG)
        std::complex<double> t = 1. / (pow(mass()->value(), 2) - model()->fourMomenta()->m2(d, pc) - Complex_i * mass()->value() * width()->value());

        T()->setValue(t, d, symIndex, sm);

        DEBUG("BreitWigner::amplitude :: calculated T = " << t << " and stored it in the cache");
        return t;
    }

    DEBUG("BreitWigner::amplitude - using cached T = " << T()->value(d, symIndex));

    // else return cached value
    return T()->value(d, symIndex);
}

//-------------------------
bool BreitWigner::consistent() const
{
    bool C = MassShapeWithNominalMass::consistent();

    if (width()->value() <= 0) {
        FLOG(ERROR) << "width <= 0";
        C &= false;
    }

    return C;
}

}




