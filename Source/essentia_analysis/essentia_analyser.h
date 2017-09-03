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

using namespace essentia;
using namespace standard;
/*
 Using Compile-Time PolyMorphism
*/
//===================================================================
template<class Derived, class ... Args>
class Analyser {
public:
    bool m_isEnabled { true };
    std::tuple<Args...> outputs;
    std::unique_ptr<Algorithm> algorithm;
//public:
    
    Analyser(){
        static_assert(sizeof...(Args)==Derived::NUM_OUTPUTS,
                      "NUMBER OF PARAMS AND OUTPUTS DON'T MATCH");
    }

    void setAlgorithm(Algorithm* newAlgorithm) {
        algorithm = std::move(std::unique_ptr<Algorithm> (newAlgorithm));
        this->setOutputsFromAlgorithm();
    }
/* ----------------- Input --------------------*/
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
/* ----------------- Enable  --------------------*/
    void setEnabled(bool shouldEnable) {
        m_isEnabled = shouldEnable;
    }
/* ----------------- Output --------------------*/
    template <int param> auto
    output () -> decltype(std::get<param> (outputs))
    {
        return std::get<param> (outputs);
    }
/* ----------------- Set Outputs --------------------*/
    template<int I = 0, int Max = 0, class Coll>
    typename std::enable_if<(I == Max), void>::type
    setOutputs(Coll& ) {}
    
    template<int I = 0, int Max = 0, class Coll>
    typename std::enable_if<(I < Max), void>::type
    setOutputs(Coll& coll)
    {
        algorithm->output(coll[I]).set(output<I>());
//        cout << "output:: " << " key : "<< coll[I] << " value: " << output<I>() << endl;
        setOutputs<I + 1, Max>(coll);
    }
    void setOutputsFromAlgorithm() {
        auto names = algorithm->outputNames();
        setOutputs<0, Derived::NUM_OUTPUTS, decltype(names)>(names);
    }
/* ----------------- Compute --------------------*/
    void compute() {
        if (m_isEnabled)
            try {
                algorithm->compute();
            } catch (EssentiaException e) {
                std::cout << e.what() << std::endl;
            }
    }
};


#endif /* essentia_analysers_h */
