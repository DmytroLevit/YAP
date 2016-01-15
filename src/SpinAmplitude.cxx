#include "SpinAmplitude.h"

#include "ClebschGordan.h"
#include "Exceptions.h"
#include "logging.h"

namespace yap {

//-------------------------
SpinAmplitude::SpinAmplitude(const QuantumNumbers& initial,
                             const QuantumNumbers& final1,
                             const QuantumNumbers& final2,
                             unsigned l, unsigned two_s,
                             InitialStateParticle* isp) :
    StaticDataAccessor(isp),
    InitialQuantumNumbers_(initial),
    FinalQuantumNumbers_( {final1, final2}),
                      L_(l),
                      TwoS_(two_s)
{
    // check JLS triangle
    if (!triangle(InitialQuantumNumbers_.twoJ(), 2 * L_, TwoS_))
        throw exceptions::AngularMomentumNotConserved("SpinAmplitude::SpinAmplitude");

    // check j1j2S triangle
    if (!triangle(FinalQuantumNumbers_[0].twoJ(), FinalQuantumNumbers_[1].twoJ(), TwoS_))
        throw exceptions::AngularMomentumNotConserved("SpinAmplitude::SpinAmplitude");

    // if (!conserves(InitialQuantumNumbers_.twoJ(), FinalQuantumNumbers_[0].twoJ(), FinalQuantumNumbers_[1].twoJ(), l))
    //     throw exceptions::AngularMomentumNotConserved();

    // check charge conservation
    if (InitialQuantumNumbers_.Q() != FinalQuantumNumbers_[0].Q() + FinalQuantumNumbers_[1].Q())
        throw exceptions::Exception(std::string("charge conservation violated: ")
                                    + "(" + std::to_string(InitialQuantumNumbers_.Q())  + ") -> "
                                    + "(" + std::to_string(FinalQuantumNumbers_[0].Q()) + ") + "
                                    + "(" + std::to_string(FinalQuantumNumbers_[1].Q()) + ")",
                                    "SpinAmplitude::SpinAmplitude");
}

//-------------------------
void SpinAmplitude::calculate(DataPoint& d)
{
    // use a default dataPartitionIndex of 0 always
    const int dPI = 0;

    // set calculation statuses uncalc'ed
    for (auto& a : amplitudeSet())
        a->setCalculationStatus(kUncalculated, dPI);

    // loop over particle combinations
    for (auto& pc : particleCombinations()) {

        unsigned symIndex = symmetrizationIndex(pc);

        // loop over mapping of parent spin projection to AmplitudeSubmap
        for (auto& aM_kv : Amplitudes_)
            // loop over mappin of daughter spin projection pairs to amplitudes
            for (auto& aSM_kv : aM_kv.second)
                // if yet uncalculated
                if (aSM_kv.second->calculationStatus(pc, symIndex, 0) == kUncalculated) {

                    const auto& two_M = aM_kv.first; // parent spin projection
                    const auto& spp = aSM_kv.first; // SpinProjectionPair of daughters

                    aSM_kv.second->setValue(calc(two_M, spp[0], spp[1], d, pc), d, symIndex, 0);

                }
    }
}

//-------------------------
SpinAmplitude::operator std::string() const
{
    std::string s = to_string(InitialQuantumNumbers_) + " -> ";
    for (auto& d : FinalQuantumNumbers_)
        s += to_string(d) + " + ";
    s.erase(s.size() - 2, 2);
    s += "with L = " + std::to_string(L_);
    s += " and S = " + spin_to_string(TwoS_);
    return s;
}

//-------------------------
std::set<int> SpinAmplitude::twoM() const
{
    std::set<int> S;
    // loop over amplitudes, key = 3-array
    for (auto& kv : Amplitudes_)
        S.insert(kv.first);  // first entry is (twice) parent spin projection
    return S;
}

//-------------------------
bool SpinAmplitude::equals(const SpinAmplitude& B) const
{
    return symmetrizationIndices() == B.symmetrizationIndices()
           // compare only spin of QuantumNumbers
           and InitialQuantumNumbers_.twoJ() == B.InitialQuantumNumbers_.twoJ()
           and FinalQuantumNumbers_[0].twoJ() == B.FinalQuantumNumbers_[0].twoJ()
           and FinalQuantumNumbers_[1].twoJ() == B.FinalQuantumNumbers_[1].twoJ()
           and L_ == B.L_
           and TwoS_ == B.TwoS_;
}

//-------------------------
CachedDataValueSet SpinAmplitude::amplitudeSet()
{
    CachedDataValueSet V;
    // loop over mapping of parent spin projection to AmplitudeSubmap
    for (auto& aM_kv : Amplitudes_)
        // loop over mappin of daughter spin projection pairs to amplitudes
        for (auto& aSM_kv : aM_kv.second)
            V.insert(aSM_kv.second);
    return V;
}

//-------------------------
std::string to_string(const SpinAmplitudeVector& saV)
{
    if (saV.empty())
        return std::string();

    std::string s = to_string(saV[0]->initialQuantumNumbers()) + " -> ";
    for (auto& d : saV[0]->finalQuantumNumbers())
        s += to_string(d) + " + ";
    s.erase(s.size() - 2, 2);
    s += "with LS =";
    for (auto& sa : saV)
        s += " (" + std::to_string(sa->L()) + ", " + spin_to_string(sa->twoS()) + "),";
    s.erase(s.size() - 1, 1);
    if (saV[0]->formalism().empty())
        return s;
    s += " in " + saV[0]->formalism();
    return s;
}

}
