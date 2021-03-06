/*
* Copyright(C) 2016, Blake C. Lucas, Ph.D. (img.science@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef NEURON_H_
#define NEURON_H_
#include <AlloyMath.h>
#include <AlloyOptimizationMath.h>
#include "NeuronFunction.h"
#include <memory>
#include <map>
namespace tgr {
	class NeuralLayer;
	struct Terminal {
		int x;
		int y;
		NeuralLayer* layer;
		Terminal(int x=0, int y=0,  NeuralLayer* l=nullptr) :x(x), y(y), layer(l) {

		}
		Terminal(int x = 0, int y = 0,  const std::shared_ptr<NeuralLayer>& l = nullptr) :x(x), y(y),  layer(l.get()) {

		}
		bool operator ==(const Terminal & r) const;
		bool operator !=(const Terminal & r) const;
		bool operator <(const Terminal & r) const;
		bool operator >(const Terminal & r) const;
	};
	class Neuron;
	class Signal {
	protected:
		static int64_t ID_COUNT;
	public:
		float* weight;
		float* change;
		int64_t id;
		std::map<const Neuron*,std::vector<Neuron*>> forwardMapping;
		std::map<const Neuron*, std::vector<Neuron*>> backwardMapping;
		Signal() :weight(nullptr),change(nullptr), id(ID_COUNT++) {

		}
		static void ResetIdCount() {
			ID_COUNT = 0;
		}
		Signal(const Signal& sig) :weight(sig.weight), change(sig.change), id(sig.id) {

		}
		Signal& operator=(const Signal& other)
		{
			weight = other.weight;
			change = other.change;
			id = other.id;
			return *this;
		}
		std::vector<Neuron*>& getForward(const Neuron* n) {
			return forwardMapping.at(n);
		}
		const std::vector<Neuron*>& getForward(const Neuron* n) const {
			return forwardMapping.at(n);
		}
		std::vector<Neuron*>& getBackward(const Neuron* n) {
			return backwardMapping.at(n);
		}
		const std::vector<Neuron*>& getBackward(const Neuron* n) const {
			return backwardMapping.at(n);
		}
		size_t size(const Neuron* n) const {
			return forwardMapping.at(n).size();
		}
		void add(Neuron* in,Neuron* out) {
			forwardMapping[out].push_back(in);
			backwardMapping[in].push_back(out);
		}
	};

	typedef std::shared_ptr<Signal> SignalPtr;
	class Neuron {
	protected:
		NeuronFunction transform;
		std::vector<SignalPtr> input;
		std::vector<SignalPtr> output;
	public:
		float* value;
		float* change;
		bool active;
		//aly::int3 id;
		friend class NeuralLayer;
		float normalizedValue() const {
			return aly::clamp((*value - transform.min()) / std::max(1E-10f,transform.max() - transform.min()),0.0f,1.0f);
		}
		float forward(float val) const {
			return transform.forward(val);
		}
		float forwardChange(float val) const {
			return transform.change(val);
		}
		size_t getInputWeightSize() const {
			return input.size();
		}
		size_t getOutputWeightSize() const {
			return output.size();
		}
		size_t getInputNeuronSize() const {
			size_t count = 0;
			for (auto in : input) {
				count += in->size(this);
			}
			return count;
		}
		void print();
		std::string getType() const;
		size_t getOutputNeuronSize() const {
			return output.size();
		}
		float evaluate();
		float backpropagate();
		const std::vector<SignalPtr>& getInput() const {
			return input;
		}
		const std::vector<SignalPtr>& getOutput() const {
			return output;
		}
		std::vector<SignalPtr>& getInput(){
			return input;
		}
		std::vector<SignalPtr>& getOutput() {
			return output;
		}
		const SignalPtr& getInput(size_t idx) const {
			return input[idx];
		}
		const SignalPtr& getOutput(size_t idx) const {
			return output[idx];
		}
		SignalPtr& getInput(size_t idx) {
			return input[idx];
		}
		SignalPtr& getOutput(size_t idx) {
			return output[idx];
		}
		std::vector<Neuron*> getInputNeurons() const;
		void getInputNeurons(std::vector<Neuron*>& out) const;
		std::vector<const Neuron*> getOutputNeurons() const;
		void getOutputNeurons(std::vector<const Neuron*>& out) const;
		
		void addInput(const SignalPtr& s) {
			input.push_back(s);
		}
		void addOutput(const SignalPtr& s) {
			output.push_back(s);
		}
		float getInputWeight(size_t idx) {
			return *input[idx]->weight;
		}

		float getOutputWeight(size_t idx) {
			return *output[idx]->weight;
		}

		Neuron(const NeuronFunction& func = ReLU());
		void setFunction(const NeuronFunction& func) {
			transform = func;
		}
	};
	class Bias : public Neuron {
		public:		
			Bias() :Neuron(Constant(1.0f)) {

			}
	};
	void MakeConnection(Neuron* src, const std::shared_ptr<Signal>& signal,Neuron* dest);
	std::shared_ptr<Signal> MakeConnection(Neuron* src, Neuron* dest);
}
#endif