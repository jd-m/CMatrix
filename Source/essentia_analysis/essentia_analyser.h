//
//  essentia_analysers.h
//  essentia_test
//
//  Created by Jaiden Muschett on 02/09/2017.
//
//

#ifndef essentia_analysers_h
#define essentia_analysers_h

#include <essentia/essentia.h>
#include <essentia/algorithm.h>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/pool.h>
#include <type_traits>

#include "jdHeader.h"
#include "essentia_analysis_gates.h"
/*
 Using Compile-Time PolyMorphism to avoid virtual calls in audio loop
*/
//===================================================================
template<class Derived, class ... Args >
class Analyser {
    using Algorithm = essentia::standard::Algorithm;
public:
    bool m_isEnabled { true };
    std::tuple<Args...> outputs;
    
    std::size_t numDetectors { jd::num_floats_args<0, Args...>() };
    
    std::unique_ptr<Algorithm> algorithm;
    
    Derived& asDerived () { return (*static_cast<Derived*>(this)); }
//public:
    
    Analyser(){
        static_assert(sizeof...(Args)==Derived::NUM_OUTPUTS,
                      "NUMBER OF OUTPUTS GIVEN AND OUTPUTS DEFINED DON'T MATCH");
    }

    void setAlgorithm(Algorithm* newAlgorithm) {
        algorithm = std::move(std::unique_ptr<Algorithm> (newAlgorithm));
        this->setOutputsFromAlgorithm();
    }
/* ----------------- Input ---------------------*/
    template<typename K, typename V>
    void setInputs(K k, V& v)
    {
        algorithm->input(k).set(v);
//        cout << algorithm->name() << ":: key: " << k << " value: " << v << endl;
    }
    
    template<typename K, typename V, class...Pairs>
    void setInputs(K k, V& v, Pairs&& ...pairs)
    {
        setInputs(k, v);
        setInputs(std::forward<Pairs>(pairs)...);
    }
/* ----------------- Params --------------------*/
    template<class ...Pairs>
    void setParams(Pairs&& ...pairs)
    {
        algorithm->configure(std::forward<Pairs>(pairs)...);
    }
/* ----------------- Enable  -------------------*/
    void setEnabled(bool shouldEnable) {
        m_isEnabled = shouldEnable;
    }
/* ----------------- Output --------------------*/
    template <int param>
    auto output () -> decltype(std::get<param> (outputs))
    {
        return std::get<param> (outputs);
    }
/* ----------------- Set Outputs ---------------*/
    template<int I = 0, int Max = 0, class Coll>
    typename std::enable_if<(I == Max),
    void>::type setOutputs(Coll& ) {}
    
    template<int I = 0, int Max = 0, class Coll>
    typename std::enable_if<(I < Max),
    void>::type setOutputs(Coll& coll)
    {
        algorithm->output(coll[I]).set(output<I>());
//        cout << "output:: " << " key : "<< coll[I] << " value: " << output<I>() << endl;
        setOutputs<I + 1, Max>(coll);
    }
    void setOutputsFromAlgorithm() {
        auto names = algorithm->outputNames();
        setOutputs<0, Derived::NUM_OUTPUTS, decltype(names)>(names);
    }
/*--------------------GATING--------------------*/
    
/* ----------------- Compute ------------------*/

    auto outValues () -> std::vector<float>
    {
        auto floatCollector = jd::tuple_element_collector<float,size_t>(numDetectors);
        jd::for_each_of_type_in_tuple<float>(std::forward<decltype(outputs)>(outputs), floatCollector);
        return floatCollector.values;
    }
    
    void compute() noexcept {
        using namespace essentia;
        if (m_isEnabled)
            try {
                algorithm->compute();
            } catch (EssentiaException e) {
                std::cout << e.what() << std::endl;
            }
//        const auto& gateInputs = outValues();
        
//        for (int i = 0; i < gateInputs.size(); i++)
//            gates[i].processSample(gateInputs[i]);
    }
};


#endif /* essentia_analysers_h */
