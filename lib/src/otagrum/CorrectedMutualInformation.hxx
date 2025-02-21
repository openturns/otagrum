#ifndef OTAGRUM_CORRECTEDMUTUALINFORMATION_HXX
#define OTAGRUM_CORRECTEDMUTUALINFORMATION_HXX

#include <agrum/base/core/hashTable.h>

#include <openturns/Sample.hxx>
#include <openturns/NormalCopula.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/IndicesManip.hxx"

namespace OTAGRUM
{

class OTAGRUM_API CorrectedMutualInformation : public OT::Object
{
public:
  // Constructor
  explicit CorrectedMutualInformation(const OT::Sample &data);

  void setAlpha(const double alpha);
  double getAlpha() const;

  OT::UnsignedInteger getSize() const;
  OT::UnsignedInteger getDimension() const;
  OT::Description getDataDescription() const;

  void clearCaches() const;

  double compute2PtCorrectedInformation(const OT::UnsignedInteger X,
                                        const OT::UnsignedInteger Y,
                                        const OT::Indices &U = OT::Indices());
  double compute2PtCorrectedInformation(const OT::Indices &X, const OT::Indices &Y);
  double compute3PtCorrectedInformation(const OT::UnsignedInteger X,
                                        const OT::UnsignedInteger Y,
                                        const OT::UnsignedInteger Z,
                                        const OT::Indices &U = OT::Indices());

  enum class KModeTypes {NoCorr, Naive};
  enum class CModeTypes {Gaussian, Bernstein};

  void setKMode(CorrectedMutualInformation::KModeTypes kmode);
  void setCMode(CorrectedMutualInformation::CModeTypes cmode);

  KModeTypes getKMode();
  CModeTypes getCMode();

private:
  void clearHCache() const;

  std::string GetKey(OT::Indices l) const;
  std::string GetKey(OT::Indices l, const OT::UnsignedInteger k) const;
  OT::UnsignedInteger GetK(const OT::UnsignedInteger size,
                           const OT::UnsignedInteger dimension);


  double computeCrossEntropy(const OT::Indices &variables, const int K);

  double compute2PtInformation(const OT::UnsignedInteger X,
                               const OT::UnsignedInteger Y,
                               const OT::Indices &U = OT::Indices());
  double compute2PtInformation(const OT::Indices &X, const OT::Indices &Y);

  double compute2PtPenalty(
    // Commented until a better correction is found
    //const OT::UnsignedInteger X,
    //const OT::UnsignedInteger Y,
    //const OT::Indices &U = OT::Indices()
  );

  double compute3PtInformation(const OT::UnsignedInteger X,
                               const OT::UnsignedInteger Y,
                               const OT::UnsignedInteger Z,
                               const OT::Indices &U = OT::Indices());
  double compute3PtPenalty(
    // Commented until a better correction is found
    //const OT::UnsignedInteger X,
    //const OT::UnsignedInteger Y,
    //const OT::UnsignedInteger Z,
    //const OT::Indices &U = OT::Indices()
  );

  mutable gum::HashTable< std::string, double > HCache_;
  OT::Sample data_;
  KModeTypes kmode_{KModeTypes::Naive};
  CModeTypes cmode_{CModeTypes::Bernstein};
  double alpha_ = 0.01;
};

}

#endif // OTAGRUM_CORRECTEDMUTUALINFORMATION_HXX
