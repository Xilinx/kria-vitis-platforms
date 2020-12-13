#!/usr/bin/env python
# Copyright 2019 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
from vai.dpuv1.rt.xdnn_util import DefaultOrderedDict
from vai.dpuv1.rt.device.layer_fpga_pyfunc import layer_fpga_pyfunc as _layer_fpga_pyfunc


fpga_layer_cnt = 0

class FPGATransform(object):
    def __init__(self, xclbin, quant_cfgfile, framework, boundryMap, filename):
        print("Creating \"FPGA\" transformation....")
        self.constSet = set()
        self.variables = {}
        self.framework = framework
        self.filename = filename

        # update schedule with new FPGA schedule
        self._layers = self._make_fpga_layer(xclbin, quant_cfgfile, boundryMap)

    def _make_netcfg(self):
        compiler_cfgfile = self.filename.rsplit('.', 1)[0] + '-netcfg.json'

        if os.path.isfile(self.filename + '.json'):
          os.rename(self.filename + '.json', compiler_cfgfile)
        elif not os.path.isfile(compiler_cfgfile):
          raise RuntimeError('Command json is not created!')

        return compiler_cfgfile

    def _make_quantCfg(self, quant_cfgfile):
        if quant_cfgfile and quant_cfgfile.rsplit('.', 1)[-1].lower() == 'json':
          return quant_cfgfile

        if quant_cfgfile.lower() == 'ignore':
          ## NOTE: hack to bypass quant file during partitioning pytest
          return

        quant_cfgfile = self.filename.rsplit('.', 1)[0] + '-quantcfg.json'

        if os.path.isfile(self.filename + '_quant.json'):
          os.rename(self.filename + '_quant.json', quant_cfgfile)
        elif not os.path.isfile(quant_cfgfile):
          raise RuntimeError('quantization json is not created!')

        return quant_cfgfile

    def _make_weights(self):
        weightFile = self.filename.rsplit('.', 1)[0] + '-data.h5'

        if os.path.isfile(self.filename + '_data.h5'):
          os.rename(self.filename + '_data.h5', weightFile)
        elif not os.path.isfile(weightFile):
          raise RuntimeError('weight directory is not created!')

        return weightFile

    def _make_fpga_layer(self, xclbin, quant_cfgfile, boundryMap):
        global fpga_layer_cnt
        args = {}
        args['xclbin'] = xclbin
        args['scaleA'] = 10000
        args['scaleB'] = 30
        args['PE'] = -1
        args['netcfg'] = self._make_netcfg()
        args['quantizecfg'] = self._make_quantCfg(quant_cfgfile)
        args['weights'] = self._make_weights()

        l = _layer_fpga_pyfunc(args, boundryMap)

        l.setName = 'fpga_pydot_layer_{}'.format(fpga_layer_cnt)
        l.setInput([name for name, shape in boundryMap['inputs']])
        l.setOutput([name for name, shape in boundryMap['outputs']])

        fpga_layer_cnt += 1

        return [l]

    def getLayers(self):
        return self._layers

    def getLayerNames(self):
        return [layer.output for layer in self._layers]
