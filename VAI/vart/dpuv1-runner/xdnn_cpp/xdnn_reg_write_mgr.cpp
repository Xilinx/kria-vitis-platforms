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
/*
 * xdnn_reg_write_mgr.cpp
 *
 *  Created on: Apr 3, 2019
 *      Author: vincentm
 */

#include "xdnn_reg_write_mgr.h"
#include "CL/cl_ext_xilinx.h"
#include "experimental/xrt++.hpp"

#include <iostream>

namespace xdnn {


unsigned int RegisterWriteManager::BitCount(unsigned int value)
{
	unsigned int result = 0;
	while (value){
		result++;
		value = value >> 1;
	}
    return result;
}

// TODO : make dynamic (constexpr)
unsigned int RegisterWriteManager::_miMaxValidElements = (((1 << 11) - 2)  >> 1);

RegisterWriteManager::RegisterWriteManager(xrt_device* xdev):
		_mbClearOnWait(true), _miNumValidElements(0), _mxdev(xdev), _miCUidx(-1)
{
	// max number of pairs per packet
	ert_packet pkt;
	pkt.count = 0;
	pkt.count = ~pkt.count;
	unsigned int size = (1 << 11); // TODO : make dynamic
	//unsigned int size = RegisterWriteManager::BitCount(pkt.count);
	if (size > 1) {
		size-=2; // for header and CU bit mask
		size = size >> 1; // divide by two <addr,value> is one entry
	}
	this->_mVectorAddrValuePairs.resize(size);
	this->_mcmd = std::shared_ptr<exec_write_cmd>(new exec_write_cmd(xdev));
	this->_mxdev = xdev;
	// C++ 14
	//this->_mcmd = make_shared<exec_write_cmd>(xdev);
}

RegisterWriteManager::~RegisterWriteManager() {
}

bool RegisterWriteManager::addAddrValuePair(addr_type addr, value_type value){
	auto rtn = false;
	pairs_container& pairslist = this->getPairs();
	auto numValidElements = this->getNumValidElements();
	if (numValidElements < pairslist.size()){
		pairslist[numValidElements] = std::make_pair(addr, value);
		this->incNumValidElements();
		rtn = true;
	}
	return rtn;
}

void RegisterWriteManager::execute(value_type cuidx){
	//xrtcpp::acquire_cu_context(this->getXdev(),cuidx);
	this->setCUidx(cuidx);
	exec_write_cmd* cmd = this->getCmd();
	const pairs_container& pairslist = this->getPairs();
	auto numValidElements = this->getNumValidElements();
	for(int i = 0; i < numValidElements; i++){
		auto pair = pairslist[i];
		cmd->add(pair.first, pair.second);
	}
	cmd->add_cu(cuidx);
	cmd->execute();
}

void RegisterWriteManager::wait(){
	if (_miCUidx < 0){
		throw std::runtime_error("RegisterWriteManager Invalid CU idx");
	}
	exec_write_cmd* cmd = this->getCmd();
	cmd->wait();
	if(this->getClearOnWait()){
	  this->clearPairs();
	  //this->_mcmd = std::shared_ptr<exec_write_cmd>(new exec_write_cmd(this->xdev));
	  //cmd->clear();
	}
	else{
		throw std::runtime_error("NOT SUPPORTED");
	}
	//xrtcpp::release_cu_context(this->getXdev(),this->getCUidx());
}

unsigned int RegisterWriteManager::getNumValidElements() const{
	return this->_miNumValidElements;
}

void RegisterWriteManager::setNumValidElements(unsigned int value){
	this->_miNumValidElements = value;
}

void RegisterWriteManager::incNumValidElements(unsigned int value){
	this->setNumValidElements(this->getNumValidElements() + value);
}

unsigned int RegisterWriteManager::getMaxValidElements() const{
	return this->_miMaxValidElements;
}

void RegisterWriteManager::setMaxValidElements(unsigned int value){
	this->_miMaxValidElements = value;
}

RegisterWriteManager::exec_write_cmd* RegisterWriteManager::getCmd() const{
	return this->_mcmd.get();
}

void RegisterWriteManager::clearPairs(){
	this->setNumValidElements(0);
}

const RegisterWriteManager::pairs_container& RegisterWriteManager::getPairs() const{
	return this->_mVectorAddrValuePairs;
}

RegisterWriteManager::pairs_container& RegisterWriteManager::getPairs(){
	return const_cast<RegisterWriteManager::pairs_container&>(const_cast<const RegisterWriteManager*>(this)->getPairs());
}

bool RegisterWriteManager::getClearOnWait() const{
	return this->_mbClearOnWait;
}

void RegisterWriteManager::setClearOnWait(){
	this->setClearOnWait(true);
}

void RegisterWriteManager::resetClearOnWait(){
	this->setClearOnWait(false);
}

void RegisterWriteManager::toggleClearOnWait(){
	this->setClearOnWait(!this->getClearOnWait());
}

void RegisterWriteManager::setClearOnWait(bool value){
	this->_mbClearOnWait = value;
}

xrt_device* RegisterWriteManager::getXdev() const{
	return this->_mxdev;
}

void RegisterWriteManager::setXdev(xrt_device* value){
	this->_mxdev = value;
}

int RegisterWriteManager::getCUidx() const{
	return this->_miCUidx;
}

void RegisterWriteManager::setCUidx(int value) {
	this->_miCUidx = value;
}

} // namespace
