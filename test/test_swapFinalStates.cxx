#include <catch.hpp>
#include <catch_capprox.hpp>

#include <BreitWigner.h>
#include <FinalStateParticle.h>
#include <FourMomenta.h>
#include <FourVector.h>
#include <HelicityAngles.h>
#include <HelicityFormalism.h>
#include <logging.h>
#include <MassAxes.h>
#include <Model.h>
#include <ParticleFactory.h>
#include <Resonance.h>
#include <ZemachFormalism.h>

#include <cmath>

/**
 * Test that the amplitude remains the same after swapping the order of the final state particles
 */

yap::MassAxes populate_model(yap::Model& M, const yap::ParticleFactory& F, const std::vector<int>& FSP)
{
    // create and set final-state particles
    M.setFinalState({F.fsp(FSP[0]), F.fsp(FSP[1]), F.fsp(FSP[2])});

    // find FSP's
    unsigned i_piPlus = FSP.size();
    unsigned i_kPlus = FSP.size();
    unsigned i_kMinus = FSP.size();
    for (size_t i = 0; i < FSP.size(); ++i)
        if (FSP[i] == F.pdgCode("pi+"))
            i_piPlus = i;
        else if (FSP[i] == F.pdgCode("K+"))
            i_kPlus  = i;
        else if (FSP[i] == F.pdgCode("K-"))
            i_kMinus = i;
    auto piPlus = M.finalStateParticles()[i_piPlus];
    auto kPlus = M.finalStateParticles()[i_kPlus];
    auto kMinus = M.finalStateParticles()[i_kMinus];

    // create ISP
    auto D = F.decayingParticle(F.pdgCode("D+"), 3.);

    // create resonances

    auto piK0 = yap::Resonance::create(yap::QuantumNumbers(0, 0), 0.75, "piK0", 3., std::make_shared<yap::BreitWigner>(0.025));
    piK0->addChannel({piPlus, kMinus});
    D->addChannel({piK0, kPlus})->freeAmplitudes()[0]->setValue(0.5 * yap::Complex_1);

    auto piK1 = yap::Resonance::create(yap::QuantumNumbers(2, 0), 1.00, "piK1", 3., std::make_shared<yap::BreitWigner>(0.025));
    piK1->addChannel({piPlus, kMinus});
    D->addChannel({piK1, kPlus})->freeAmplitudes()[0]->setValue(1. * yap::Complex_1);

    auto piK2 = yap::Resonance::create(yap::QuantumNumbers(4, 0), 1.25, "piK2", 3., std::make_shared<yap::BreitWigner>(0.025));
    piK2->addChannel({piPlus, kMinus});
    D->addChannel({piK2, kPlus})->freeAmplitudes()[0]->setValue(30. * yap::Complex_1);

    return M.massAxes({{i_piPlus, i_kMinus}, {i_kMinus, i_kPlus}});
}

std::complex<double> calculate_model(yap::Model& M, const yap::MassAxes& A, std::vector<double> m2)
{
    // calculate four-momenta
    auto P = M.calculateFourMomenta(A, m2);

    // if failed, outside phase space
    if (P.empty())
        return std::numeric_limits<double>::quiet_NaN();

    // create new data set
    auto data = M.dataSet();
    // add point
    data.add(P);
    // return amplitude
    return M.amplitude(data[0], data);
}

TEST_CASE( "swapFinalStates" )
{

    // disable logs in text
    yap::disableLogs(el::Level::Global);
    //yap::plainLogs(el::Level::Global);

    auto F = yap::ParticleFactory((std::string)::getenv("YAPDIR") + "/data/evt.pdl");

    // create models
    std::vector<yap::Model*> Z;     // Zemach
    std::vector<yap::MassAxes> mZ; // always (pi+, K-), (K-, K+)
    std::vector<yap::Model*> H;     // Helicity
    std::vector<yap::MassAxes> mH; // always (pi+, K-), (K-, K+)

    std::vector<int> FSP = {F.pdgCode("K-"), F.pdgCode("pi+"), F.pdgCode("K+")};
    std::sort(FSP.begin(), FSP.end());
    do {

        // Zemach
        Z.emplace_back(new yap::Model(std::make_unique<yap::ZemachFormalism>()));
        mZ.push_back(populate_model(*Z.back(), F, FSP));

        // Helicity
        H.emplace_back(new yap::Model(std::make_unique<yap::HelicityFormalism>()));
        mH.push_back(populate_model(*H.back(), F, FSP));

    } while (std::next_permutation(FSP.begin(), FSP.end()));

    // get piK and KK mass ranges
    auto m2_piK_range = Z[0]->massRange(mZ[0][0]);
    auto m2_KK_range  = Z[0]->massRange(mZ[0][1]);

    const unsigned N = 20;
    for (double m2_piK = m2_piK_range[0]; m2_piK <= m2_piK_range[1]; m2_piK += (m2_piK_range[1] - m2_piK_range[0]) / N) {
        for (double m2_KK = m2_KK_range[0]; m2_KK <= m2_KK_range[1]; m2_KK += (m2_KK_range[1] - m2_KK_range[0]) / N) {

            std::vector<std::complex<double> > amps_Z(Z.size(), 0.);
            std::vector<std::complex<double> > amps_H(H.size(), 0.);

            for (size_t i = 0; i < Z.size(); ++i) {
                amps_Z[i] = calculate_model(*Z[i], mZ[i], {m2_piK, m2_KK});
                amps_H[i] = calculate_model(*H[i], mH[i], {m2_piK, m2_KK});

                // check equality between Zemach and Helicity
                // REQUIRE( amps_Z[i] == CApprox( amps_H[i] ) );
            }

            std::cout << m2_piK << ", " << m2_KK << " is "
                      << ((std::isnan(real(amps_Z[0]))) ? "out" : "in") << " phase space" << std::endl;

            // if (!std::isnan(real(amps_Z[0]))) {

            //     for (size_t i = 0; i < H.size(); ++i)  {
            //         std::cout << *mH[i][0] << std::flush;
            //         std::cout << " = " << H[i]->helicityAngles()->symmetrizationIndex(mH[i][0]) << std::endl;
            //     }
            //     //     std::cout << "("  << H[i]->helicityAngles()->theta(H[i]->dataSet()[0], mH[i][0])
            //     //               << ", " << H[i]->helicityAngles()->phi(H[i]->dataSet()[0], mH[i][0])
            //     //               << "    ";
            //     // std::cout << std::endl;
            //     // for (size_t i = 0; i < H.size(); ++i)
            //     //     std::cout << "("  << H[i]->helicityAngles()->theta(H[i]->dataSet()[0], mH[i][1])
            //     //               << ", " << H[i]->helicityAngles()->phi(H[i]->dataSet()[0], mH[i][1])
            //     //               << "    ";
            //     // std::cout << std::endl;

            // }

            // print
            // std::cout << "Zemach:                  Helicity:" << std::endl;
            // for (size_t i = 0; i < amps_Z.size(); ++i)
            //     std::cout << amps_Z[i] << "    " << amps_H[i] << std::endl;
            for (size_t i = 0; i < H.size(); ++i) {
                auto PCs = H[i]->helicityAngles()->particleCombinations();
                std::cout << "piK = " << *mH[i][0];
                for (size_t j = 0; j < PCs.size(); ++j) {
                    std::cout << "\t :: " << *PCs[j] << ": "
                              << "(" << H[i]->helicityAngles()->phi(H[i]->dataSet()[0], PCs[j]) * yap::deg_per_rad<double>()
                              << ", " << H[i]->helicityAngles()->theta(H[i]->dataSet()[0], PCs[j]) * yap::deg_per_rad<double>()
                              << ")";
                }
                std::cout << std::endl;
            }

            // // check equality for Zemach
            // for (size_t i = 1; i < amps_Z.size(); ++i)
            //     REQUIRE ( amps_Z[i - 1] == CApprox( amps_Z[i] ) );

            // // check equality for Helicity
            // for (size_t i = 1; i < amps_H.size(); ++i)
            //     REQUIRE ( amps_H[i - 1] == CApprox( amps_H[i] ) );
        }
    }

}
