/*  YAP - Yet another PWA toolkit
    Copyright 2015, Technische Universitaet Muenchen,
    Authors: Daniel Greenwald, Johannes Rauch

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/// \file

#ifndef yap_Resonance_h
#define yap_Resonance_h

#include "DecayingParticle.h"
#include "DataAccessor.h"
#include "DataPoint.h"
#include "MassShape.h"

#include <complex>
#include <memory>

namespace yap {

class DecayChannel;
class FinalStateParticle;
class InitialStateParticle;
class ParticleCombination;

/// \class Resonance
/// \brief Class for a particle that will decay and has a mass shape
/// \author Johannes Rauch, Daniel Greenwald
/// \ingroup Particle

class Resonance : public DecayingParticle
{
public:

    /// Constructor
    /// \param q QuantumNumbers of resonance
    /// \param mass mass of resonance
    /// \param radialSize radial size of resonance
    /// \param massShape unique_ptr to MassShape of resonance
    Resonance(const QuantumNumbers& q, double mass, std::string name, double radialSize, std::unique_ptr<MassShape> massShape);

    /// Calculate complex amplitude
    /// \param d DataPoint to calculate with
    /// \param pc (shared_ptr to) ParticleCombination to calculate for
    /// \param two_m 2 * the spin projection to calculate for
    /// \param dataPartitionIndex partition index for parallelization
    virtual std::complex<double> amplitude(DataPoint& d, const std::shared_ptr<ParticleCombination>& pc, int two_m, unsigned dataPartitionIndex) const override
    { return DecayingParticle::amplitude(d, pc, two_m, dataPartitionIndex) * MassShape_->amplitude(d, pc, dataPartitionIndex); }

    /// Check consistency of object
    virtual bool consistent() const override;

    /// \name Getters
    /// @{

    /// access MassShape
    MassShape& massShape()
    { return *(MassShape_.get()); }

    /// access MassShape (const)
    const MassShape& massShape() const
    { return *(MassShape_.get()); }

    /// @}

protected:

    /// overrides DataAccessor's function to also register MassShape_ with ISP
    virtual void addToInitialStateParticle()
    {
        DecayingParticle::addToInitialStateParticle();
        MassShape_->addToInitialStateParticle();
    }

    /// add ParticleCombination to ParticleCombinations_,
    /// also add to MassShape_
    virtual void addParticleCombination(std::shared_ptr<ParticleCombination> c) override;

private:

    /// MassShape object
    std::shared_ptr<MassShape> MassShape_;

};

}

#endif
