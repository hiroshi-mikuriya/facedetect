#include "stdafx.h"
#include "features.h"

pierry::feature_points::feature_points(cv::Mat const & src, pierry::orb_params const & orb, pierry::freak_params const & freak)
{
    if(1!=src.channels()){
        throw std::invalid_argument("src channels should be 1.");
    }
    auto detector = cv::FeatureDetector::create("ORB");
    auto extractor = cv::DescriptorExtractor::create("ORB");
    //cv::OrbFeatureDetector detector(orb.nfeatures, orb.scaleFactor, orb.nlevels, orb.edgeThreshold, orb.firstLevel, orb.WTA_K, orb.scoreType, orb.patchSize);
    //cv::FREAK extractor(freak.orientationNormalized, freak.scaleNormalized, freak.patternScale, freak.nOctaves, freak.selectedPairs);
    detector->detect(src, m_key_points);
    extractor->compute(src, m_key_points, m_features);
}

