#include <BRDF/BxDF.h>

bool BxDF::MatchesFlags(BxDFType t) const
{
	return (type & t) == type;
}
