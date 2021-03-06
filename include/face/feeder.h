#pragma once

#include "io/pipe.h"
#include "feature/feature_detector.h"
#include "align/nonrigid_pipe.h"
#include "face/model.h"

namespace {
    using namespace telef::align;
}

namespace telef::face {
    class MorphableModelFeederPipe : public telef::io::Pipe<telef::feature::FittingSuite, PCANonRigidAlignmentSuite> {
    private:
        using MModelTptr = std::shared_ptr<telef::face::MorphableFaceModel>;
        using BaseT = telef::io::Pipe<telef::feature::FittingSuite, PCANonRigidAlignmentSuite>;
        using PtCldPtr = pcl::PointCloud<pcl::PointXYZ>::Ptr;

        MModelTptr pca_model;

        boost::shared_ptr<PCANonRigidAlignmentSuite>
        _processData(boost::shared_ptr<telef::feature::FittingSuite> in) override;

    public:
        explicit MorphableModelFeederPipe(MModelTptr model);
    };
}
