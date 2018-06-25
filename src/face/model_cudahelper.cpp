#include <cuda_runtime_api.h>

#include "face/model_cudahelper.h"


void loadModelToCUDADevice(C_PcaDeformModel *deformModel,
                           const Eigen::MatrixXf deformBasis, const Eigen::VectorXf ref,
                           const std::vector<int> lmkInds) {

    cudaMalloc((void**)(&deformModel->deformBasis_d), deformBasis.size()*sizeof(float));
    cudaMalloc((void**)(&deformModel->ref_d), ref.size()*sizeof(float));
    cudaMalloc((void**)(&deformModel->lmks_d), lmkInds.size()*sizeof(int));

    cudaMemcpy((void*)deformModel->deformBasis_d,
               deformBasis.data(), deformBasis.size()*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy((void*)deformModel->ref_d,
               ref.data(), ref.size()*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy((void*)deformModel->lmks_d,
               lmkInds.data(), lmkInds.size()*sizeof(int), cudaMemcpyHostToDevice);
    deformModel->rank = (int)deformBasis.cols();
    deformModel->dim = (int)deformBasis.rows();
    deformModel->lmkCount = (int)lmkInds.size();

    assert(deformBasis.rows() == ref.size());
}

void freeModelCUDA(C_PcaDeformModel deformModel) {
    cudaFree(deformModel.deformBasis_d);
    cudaFree(deformModel.ref_d);
    cudaFree(deformModel.lmks_d);
}

void loadScanToCUDADevice(C_ScanPointCloud *scanPointCloud,
                          boost::shared_ptr<const pcl::PointCloud<pcl::PointXYZRGBA>> scan) {

    cudaMalloc((void**)(&scanPointCloud->scanPoints_d), scan->points.size()*3*sizeof(float));

    float *scanPoints = new float[scan->points.size()*3];
    for (int i=0; i<scan->points.size(); i+=3) {
        scanPoints[i] = scan->points[i].x;
        scanPoints[i+1] = scan->points[i].y;
        scanPoints[i+2] = scan->points[i].z;
    }

    cudaMemcpy((void*)scanPointCloud->scanPoints_d,
               scanPoints, scan->points.size()*3*sizeof(float), cudaMemcpyHostToDevice);
}

void freeScanCUDA(C_ScanPointCloud scanPointCloud) {
    cudaFree(scanPointCloud.scanPoints_d);
}

void allocParamsToCUDADevice(C_Params *params, int numParams) {
    cudaMalloc((void **)(&params->params_d), numParams*sizeof(float));
    float *zero = new float[numParams]{0,};
    params->numParams = numParams;

    updateParamsInCUDADevice(*params, zero, numParams);
    delete[] zero;
}

void updateParamsInCUDADevice(const C_Params params, const float * const paramsIn, int numParams) {
    cudaMemcpy((void*)params.params_d, paramsIn, numParams*sizeof(float), cudaMemcpyHostToDevice);
}

void freeParamsCUDA(C_Params params) {
    cudaFree(params.params_d);
}

void allocPositionCUDA(float **position_d, int dim) {
    cudaMalloc((void**)(position_d), dim*sizeof(float));
}

void freePositionCUDA(float *position_d) {
    cudaFree(position_d);
}
