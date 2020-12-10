// Copyright 2019 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <string>
#include <vector>
#include "H5Cpp.h"
#include <unordered_map>
#include "xdnn_fcweightsload_cpp_infer.h"
#define XDNN_VERBOSE 0
bool has_suffix(const std::string &str, const std::string &suffix);

void XDNNLoadFCWeights(std::unordered_map<int, std::vector<std::vector<float>> > &fc_wb_map,char *hdf5_file){

    if (has_suffix(hdf5_file,".h5")) // Load weights and bias from HDF5 file
  {

    H5::Exception::dontPrint(); // Don't print H5 exceptions, we will handle

    const H5std_string FILE_NAME(hdf5_file);  // Open up weights
    H5::H5File file(FILE_NAME, H5F_ACC_RDONLY);  //   as hdf5 file
    int fc_start_indx=0;
    // Load up all weights
    for (int fi=0; ; fi++)
    {
      try
      {

        // Open the HDF5 Dataset for given layer
        std::string dsname = "fwbqb_" + std::to_string(fi);
        const H5std_string DATASET_NAME(dsname);
        H5::DataSet dataset = file.openDataSet(DATASET_NAME);

        // Get the dataspace // Defines number of dimensions, and the size of each dimension
        H5::DataSpace dataspace = dataset.getSpace();

        // Get the number of dimensions in the dataspace.
        int rank = dataspace.getSimpleExtentNdims();

        // Get the dimension size of each dimension in the dataspace
        std::vector<hsize_t> dims(rank);
        int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

        // Get the layerName from the dataset's layer_name attribute
        const H5std_string  ATTR_NAME("layer_name");
        H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
        H5::StrType stype = attribute.getStrType();
        std::string layerName;
	    attribute.read(stype,layerName);

        if (XDNN_VERBOSE)
        {
          std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
          std::cout << "rank: " << rank << ", dimensions: ";
          for (int i=0;i<dims.size();i++)
	      std::cout << (unsigned long)(dims[i]) << " x ";
          std::cout << std::endl;
        }

        // Get the raw data
        /*std::vector<float> weights;
        int space = 1;
        for (int i=0;i<rank;i++)
          space *= dims[i];
        weights.resize(space);
        dataset.read(&weights[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);*/

        // Uncomment below if you don't trust hdf5 weights, and you need to see them
        //std::ofstream outfile;
        //outfile.open("outfile.txt." + std::to_string(fi));
        //for (int i=0;i<space;i++)
        //  outfile << std::fixed << weights[i] << " ";

        //allWeights[layerName] = weights;
      }
      catch( H5::FileIException error )
      {
        if (XDNN_VERBOSE)
          std::cout << "No more weights in HDF5" << std::endl;
        break;
      }
	fc_start_indx=fi;
    }

    // Load up all bias
    for (int fi=0; ; fi++)
    {
      try
      {
        // Open the HDF5 Dataset for given layer
        std::string dsname = "fwbqb_bias_" + std::to_string(fi);
        const H5std_string DATASET_NAME(dsname);
        H5::DataSet dataset = file.openDataSet(DATASET_NAME);

        // Get the dataspace // Defines number of dimensions, and the size of each dimension
        H5::DataSpace dataspace = dataset.getSpace();

        // Get the number of dimensions in the dataspace.
        int rank = dataspace.getSimpleExtentNdims();

        // Get the dimension size of each dimension in the dataspace
        std::vector<hsize_t> dims(rank);
        int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

        // Get the layerName from the dataset's layer_name attribute
        const H5std_string  ATTR_NAME("layer_name");
        H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
        H5::StrType stype = attribute.getStrType();
        std::string layerName;
	    attribute.read(stype,layerName);

        if (XDNN_VERBOSE)
        {
          std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
          std::cout << "rank: " << rank << ", dimensions: ";
          for (int i=0;i<dims.size();i++)
	        std::cout << (unsigned long)(dims[i]) << " x ";
          std::cout << std::endl;
        }

        // Get the raw data
        /*std::vector<float> bias;
        int space = 1;
        for (int i=0;i<rank;i++)
          space *= dims[i];
        bias.resize(space);
        dataset.read(&bias[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);*/

        //allBias[layerName] = bias;
      }
      catch( H5::FileIException error )
      {
        if (XDNN_VERBOSE)
          std::cout << "No more bias in HDF5" << std::endl;
        break;
      }
    }
	std::vector<std::vector<float>> fc_w_vect;
        std::vector<std::vector<float>> fc_b_vect;
	   // Load up all FC weights
        for (int fc_i= fc_start_indx+1; ; fc_i++)
        {
            try
            {
                std::string dsname = "fc_" + std::to_string(fc_i);
                const H5std_string DATASET_NAME(dsname);
                H5::DataSet dataset  = file.openDataSet(DATASET_NAME);

                // Get the dataspace // Defines number of dimensions, and the size of each dimension
                H5::DataSpace dataspace = dataset.getSpace();

                // Get the number of dimensions in the dataspace.
                int rank = dataspace.getSimpleExtentNdims();

                // Get the dimension size of each dimension in the dataspace
                std::vector<hsize_t> dims(rank);
                int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

                // Get the layerName from the dataset's layer_name attribute
                const H5std_string  ATTR_NAME("layer_name");
                H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
                H5::StrType stype = attribute.getStrType();
                std::string layerName;
                attribute.read(stype,layerName);

                if (XDNN_VERBOSE)
                {
                    std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
                    std::cout << "rank: " << rank << ", dimensions: ";
                    for (int i=0;i<dims.size();i++)
                    std::cout << (unsigned long)(dims[i]) << " x ";
                    std::cout << std::endl;
                }

                // Get the raw data
                std::vector<float> fc_weights;
                int space = 1;
                for (int i=0;i<rank;i++)
                space *= dims[i];
                fc_weights.resize(space);
                dataset.read(&fc_weights[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);

                // Uncomment below if you don't trust hdf5 weights, and you need to see them
                //std::ofstream outfile;
                //outfile.open("outfile.txt." + std::to_string(fi));
                //for (int i=0;i<space;i++)
                //  outfile << std::fixed << weights[i] << " ";
                fc_w_vect.push_back(fc_weights);

            }
            catch( H5::FileIException error )
            {
                if (XDNN_VERBOSE)
                std::cout << "No more weights in HDF5" << std::endl;
                break;
            }
        }
		// Load up all FC weights
        for (int fc_i= fc_start_indx+1; ; fc_i++)
        {
            try
            {
                // Open the HDF5 Dataset for given layer
                std::string dsname = "fc_bias_" + std::to_string(fc_i);
                const H5std_string DATASET_NAME(dsname);
                H5::DataSet dataset = file.openDataSet(DATASET_NAME);

                // Get the dataspace // Defines number of dimensions, and the size of each dimension
                H5::DataSpace dataspace = dataset.getSpace();

                // Get the number of dimensions in the dataspace.
                int rank = dataspace.getSimpleExtentNdims();

                // Get the dimension size of each dimension in the dataspace
                std::vector<hsize_t> dims(rank);
                int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

                // Get the layerName from the dataset's layer_name attribute
                const H5std_string  ATTR_NAME("layer_name");
                H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
                H5::StrType stype = attribute.getStrType();
                std::string layerName;
                attribute.read(stype,layerName);
                if (XDNN_VERBOSE)
                {
                    std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
                   std::cout << "rank: " << rank << ", dimensions: ";
                   for (int i=0;i<dims.size();i++)
                       std::cout << (unsigned long)(dims[i]) << " x ";
                       std::cout << std::endl;
                }

                // Get the raw data
                std::vector<float> bias;
                int space = 1;
                for (int i=0;i<rank;i++)
                   space *= dims[i];
                bias.resize(space);
                dataset.read(&bias[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);
                fc_b_vect.push_back(bias);
            }
            catch( H5::FileIException error )
            {
                if (XDNN_VERBOSE)
                    std::cout << "No more bias in HDF5" << std::endl;
                    break;
            }
        }
        if(fc_b_vect.size()==fc_w_vect.size()){
            for(int it_fc=0;it_fc<fc_w_vect.size();it_fc++){
                std::vector<std::vector<float>> fc_wb_vect;
                fc_wb_vect.push_back(fc_w_vect[it_fc]);
                fc_wb_vect.push_back(fc_b_vect[it_fc]);
                fc_wb_map[it_fc]=fc_wb_vect;
            }
        }
  }
}
