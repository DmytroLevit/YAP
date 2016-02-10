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

#ifndef yap_Model_h
#define yap_Model_h

#include "CoordinateSystem.h"
#include "DataPartition.h"
#include "DataSet.h"
#include "FourMomenta.h"
#include "FourVector.h"
#include "HelicityAngles.h"
#include "HelicitySpinAmplitude.h"
#include "MeasuredBreakupMomenta.h"
#include "ParticleCombinationCache.h"

#include <complex>
#include <memory>
#include <vector>

namespace yap {

class DecayingParticle;
class FinalStateParticle;
class MassAxes;
class SpinAmplitudeCache;

/// \class Model
/// \brief Class implementing a PWA model
/// \author Johannes Rauch, Daniel Greenwald

class Model
{
public:

    /// Constructor
    /// \param SAC unique_ptr to SpinAmplitudeCache
    Model(std::unique_ptr<SpinAmplitudeCache> SAC);

    /// \name Amplitude-related
    /// @{

    /// \return amplitude with a sum over all particle combinations of ISP
    /// \param d DataPoint to calculate with
    /// \param two_m 2 * the spin projection of ISP to calculate for
    /// \param dataPartitionIndex partition index for parallelization
    std::complex<double> amplitude(DataPoint& d, int two_m, unsigned dataPartitionIndex) const;

    /// \return amplitude with a sum over all particle combinations and spin projections of ISP
    /// \param d DataPoint to calculate with
    /// \param dataPartitionIndex partition index for parallelization
    std::complex<double> amplitude(DataPoint& d, unsigned dataPartitionIndex) const;

    /// \return ln(|amplitude|^2), with sum over all particle combinations in amp. calculation
    /// calls resetCalculationStatuses before calculation
    double logOfSquaredAmplitude(DataPoint& d, unsigned dataPartitionIndex);

    /// \return The sum of the logs of squared amplitudes evaluated over the data partition
    /// \param D Pointer to a #DataPartitionBase object
    double partialSumOfLogsOfSquaredAmplitudes(DataPartitionBase* D);

    /// Calculate the sum of the logs of the squared amplitudes evaluated over all partitions
    double sumOfLogsOfSquaredAmplitudes();

    /// @}

    /// call before looping over all DataPartitions
    void updateGlobalCalculationStatuses();

    /// calculate StaticDataAccessors
    /// \param d DataPoint to calculate on
    /// \param dataPartitionIndex for tracking statuses
    void calculate(DataPoint& d, unsigned dataPartitionIndex = 0);

    /// Check consistency of object
    virtual bool consistent() const;

    /// removes expired DataAccessor's, prune's remaining, and assigns them indices
    void prepareDataAccessors();

    /// \name Getters
    /// @{

    /// \return coordinate system
    CoordinateSystem<double, 3>& coordinateSystem()
    { return CoordinateSystem_; }

    /// \return coordinate system (const)
    const CoordinateSystem<double, 3>& coordinateSystem() const
    { return CoordinateSystem_; }

    /// \return FourMomenta accessor
    FourMomenta& fourMomenta()
    { return *FourMomenta_; }

    /// \return FourMomenta accessor (const)
    const FourMomenta& fourMomenta() const
    { return *FourMomenta_; }

    /// \return MeasuredBreakupMomenta accessor
    MeasuredBreakupMomenta& measuredBreakupMomenta()
    { return *MeasuredBreakupMomenta_; }

    /// \return MeasuredBreakupMomenta accessor (const)
    const MeasuredBreakupMomenta& measuredBreakupMomenta() const
    { return *MeasuredBreakupMomenta_; }

    /// \return HelicityAngles accessor
    HelicityAngles& helicityAngles()
    { return *HelicityAngles_; }

    /// \return HelicityAngles accessor (const)
    const HelicityAngles& helicityAngles() const
    { return *HelicityAngles_; }

    /// \return ParticleCombinationCache
    ParticleCombinationCache& particleCombinationCache()
    { return ParticleCombinationCache_; }

    /// \return ParticleCombinationCache (const)
    const ParticleCombinationCache& particleCombinationCache() const
    { return ParticleCombinationCache_; }

    /// \return SpinAmplitudeCache
    SpinAmplitudeCache* spinAmplitudeCache()
    { return SpinAmplitudeCache_.get(); }

    /// \return SpinAmplitudeCache (const)
    const SpinAmplitudeCache* spinAmplitudeCache() const
    { return SpinAmplitudeCache_.get(); }

    /// \return Initial-state particle
    DecayingParticle* initialStateParticle()
    { return InitialStateParticle_; }

    /// \return Initial-state particle (const)
    const DecayingParticle* initialStateParticle() const
    { return InitialStateParticle_; }

    /// \return vector of shared pointers to final state particles
    const std::vector<std::shared_ptr<FinalStateParticle> >& finalStateParticles() const
    { return FinalStateParticles_; }

    /// \return (min, max) array[2] of mass range for particle combination
    /// \param pc shared pointer to ParticleCombination to get mass range of
    std::array<double, 2> getMassRange(const std::shared_ptr<ParticleCombination>& pc) const;

    /// \return free amplitudes of DecayChannels_
    ComplexParameterVector freeAmplitudes() const;

    /// @}

    /// \name Setters
    /// @{

    /// Set initial-state particle
    /// \param isp raw pointer to initial-state particle
    void setInitialStateParticle(DecayingParticle* isp);

    /// Set final-state particle content. The order in which particles
    /// are given dictates the order in which four-momenta must be
    /// given in data points. The FinalStateParticle's have their
    /// Model pointer set to this
    /// \param FSP list of shared pointers to final-state particles
    void  setFinalState(std::initializer_list<std::shared_ptr<FinalStateParticle> > FSP);

    /// set coordinate system
    void setCoordinateSystem(const CoordinateSystem<double, 3>& cs);

    /// @}

    /// \name Data set and partitions
    /// @{

    /// \return DataSet
    DataSet& dataSet()
    { return DataSet_; }

    /// \return the data partitions
    std::vector<DataPartitionBase*> dataPartitions();

    /// set data partitions
    /// ownership over DataPartitionBase objects will be taken
    void setDataPartitions(std::vector<std::unique_ptr<DataPartitionBase> > partitions);

    /// @}

    /// \name DataPoints
    /// @{

    /// Add data point via four-momenta
    /// This method is faster since it avoids unneccessary copying of objects
    /// and resizing of the DataPoint's storage
    void addDataPoint(const std::vector<FourVector<double> >& fourMomenta);

    /// Add data point via move
    /// \param d DataPoint to move into DataSet
    void addDataPoint(DataPoint&& d);

    /// Add data point via copy
    /// \param d DataPoint to copy into DataSet
    void addDataPoint(const DataPoint& d);

    /// @}

    /// \name Monte Carlo Generation
    /// @{

    /// Initialize DataSet for MC Generation
    /// \param n Number of simultaneous streams for MC generation
    void initializeForMonteCarloGeneration(unsigned n);

    /// Build vector of mass axes for coordinates in phase space.
    /// Currently only supports two-particle masses; the PCs put into
    /// the returned MassAxes will have their daughters sorted (i.e. (10) will become (01)).
    /// \return MassAxes for requested particle combinations
    /// \param pcs vector of vectors of particle indices
    const MassAxes getMassAxes(std::vector<std::vector<unsigned> > pcs);

    /// Calculate four-momenta for final-state particles for phase-space coordinate
    /// \param axes phase-space axes
    /// \param squared_masses phase-space coordinate
    std::vector<FourVector<double> > calculateFourMomenta(const MassAxes& axes, const std::vector<double>& squared_masses) const;

    /// Set fsp four-momenta of data point
    /// \param d DataPoint to set into
    /// \param P Final-state four momenta
    /// \param dataPartitionIndex for tracking status
    void setFinalStateFourMomenta(DataPoint& d, const std::vector<FourVector<double> >& P, unsigned dataPartitionIndex = 0);

    /// @}

    /// Print the list of DataAccessor's
    void printDataAccessors(bool printParticleCombinations = true);

    /// reset all CalculationStatus'es for the dataPartitionIndex to the GlobalCalculationStatus_
    /// call before calculating the amplitude for a new dataPoint
    void resetCalculationStatuses(unsigned dataPartitionIndex);

    /// grant friend status to DataAccessor to register itself with this
    friend class DataAccessor;

    /// grant friend status to DecayingParticle to call addParticleCombination
    friend class DecayingParticle;

protected:

    /// add ParticleCombination to to FourMomenta_, HelicityAngles_, and
    /// MeasuredBreakupMomenta_ (along with it's daughters through
    /// recursive calling) if it is NOT for a FSP.
    virtual void addParticleCombination(std::shared_ptr<ParticleCombination> pc);

    /// register a DataAccessor with this Model
    virtual void addDataAccessor(DataAccessorSet::value_type da);

    /* /// remove a DataAccessor from this Model */
    /* virtual void removeDataAccessor(DataAccessorSet::value_type da); */

private:

    /// check if d is in DataPartitions_
    bool hasDataPartition(DataPartitionBase* d);

    /// set number of data partitions of all #CachedDataValue's
    void setNumberOfDataPartitions(unsigned n);

    /// set all parameter flags to kUnchanged (or leave at kFixed)
    /// call after looping over a DataPartition
    void setCachedDataValueFlagsToUnchanged(unsigned dataPartitionIndex);

    /// set all parameter flags to kUnchanged (or leave at kFixed)
    /// call after looping over ALL DataPartitions
    void setParameterFlagsToUnchanged();

    /// Lab coordinate system to use in calculating helicity angles
    CoordinateSystem<double, 3> CoordinateSystem_;

    /// ParticleCombination cache
    ParticleCombinationCache ParticleCombinationCache_;

    /// SpinAmplitude cache
    std::unique_ptr<SpinAmplitudeCache> SpinAmplitudeCache_;

    /// Set of all DataAccessor's registered to this model
    DataAccessorSet DataAccessors_;

    /// Raw pointer to initial-state particle
    DecayingParticle* InitialStateParticle_;

    /// vector of final state particles
    std::vector<std::shared_ptr<FinalStateParticle> > FinalStateParticles_;

    /// four momenta manager
    std::shared_ptr<FourMomenta> FourMomenta_;

    /// Breakup momenta manager
    std::shared_ptr<MeasuredBreakupMomenta> MeasuredBreakupMomenta_;

    /// helicity angles manager
    std::shared_ptr<HelicityAngles> HelicityAngles_;

    /// Data set
    DataSet DataSet_;

    /// Data partitions
    std::vector<std::unique_ptr<DataPartitionBase> > DataPartitions_;

};

}

#endif