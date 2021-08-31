#ifndef CxAODReader_StaticMVATree_H
#define CxAODReader_StaticMVATree_H

class TBranch;
class TTree;

#include "CxAODTools/Utilities.h"

namespace StaticMVATree_internal {
// keep in case looping over MVATree needs specialization
template <typename FuncT, std::size_t I = 0, typename... Tp>
void for_each_MVATree(std::tuple<Tp...> &t, FuncT &&f) {
  f(std::get<I>(t));
  Analysis::for_each<FuncT, I + 1, Tp...>(t, std::forward<FuncT>(f));
}

template <typename FuncT, std::size_t I = 0, typename... Tp>
void for_each_MVATree(std::tuple<Tp...> &t, const FuncT &f) {
  f(std::get<I>(t));
  Analysis::for_each<FuncT, I + 1, Tp...>(t, f);
}

// name, tuple<value, default value, analysis tag, is initialized flag>
// template <class Type> using map = std::unordered_map<std::string,
// std::tuple<Type, Type, std::string, bool>>;
template <class Type>
using map = std::map<std::string, std::tuple<Type, Type, std::string, bool>,
                     Analysis::fast_string_cmp>;

} // namespace StaticMVATree_internal

// CRTP: AddBranch(const char*, T *address), m_treeMap
template <class Derived, class... Types> class StaticMVATree {
private:
  // "type", map
  std::tuple<StaticMVATree_internal::map<Types>...> m_variableMaps;
  // std::tuple<std::unordered_map<std::string, std::tuple<Types, Types,
  // std::string, bool> >...> m_variableMaps;

  // need to make functors b/c we don't use C++14 :(
  struct AddBranch {
    // Derived is indeed either MVATree or EasyTree
    // m_tree is hence a pointer to the EasyTree or MVATree object
    mutable Derived *m_tree;
    std::string m_analysisType;

    template <typename T> void operator()(T &t) const {
      // p loops over all the tuple of m_variableMaps for a given type T (for
      // example T = double or T = float) the this->for_each(addBranch); in
      // StaticSetBranches ensures the loop is over all types T contained in
      // m_variableMaps
      for (auto &p : t) {
        auto &quadruplet = p.second;

        // if std::get<3>(quadruplet) = true it means the branch has already
        // been created
        // -> no need to create it again
        if (!std::get<3>(quadruplet) &&
            ((std::get<2>(quadruplet) == "StaticMVATree::ALL") ||
             (std::get<2>(quadruplet).find(m_analysisType) !=
              std::string::npos))) {
          // create branch for all trees in m_tree
          // p.first = branch name = pTBB for example
          m_tree->AddBranch(p.first.c_str(), &std::get<0>(quadruplet));
          // set boolean of the quadruplet to true
          // -> branch has now been created for all trees
          std::get<3>(quadruplet) = true;

          // get current value
          auto val = std::get<0>(quadruplet);
          std::get<0>(quadruplet) = std::get<1>(
              quadruplet); // assign default value for "missing" entries
          // check if this branch has been skipped for the different trees; if
          // so, fill with default values
          TBranch *branch = nullptr;
          // loop over all trees Nominal,
          // JET_CR_JET_EffectiveNP_Statistical1__1down and so on
          for (std::pair<std::string, TTree *> ptree : m_tree->m_treeMap) {
            branch = ptree.second->FindBranch(p.first.c_str());
            // for each tree
            // if it already contains some events it means Fill() has been
            // called
            // -> ptree.second->GetEntries() is the number of events already
            // processed and saved in the tree hence it is the number of times
            // Fill() has been already called

            // As the current branch is new it has not been filled yet
            // -> for the already processed events fill the new branch with its
            // default value
            for (long long int i = 0; i < ptree.second->GetEntries(); i++) {
              branch->Fill();
            }
          }
          // assign again current value
          std::get<0>(quadruplet) = val;
        }
      }
    } // end of operator()
  } addBranch;

  struct Reset {
    template <typename T> void operator()(T &t) const {
      for (auto &p : t) {
        auto &quadruplet = p.second;
        std::get<0>(quadruplet) = std::get<1>(quadruplet);
      }
    } // ()
  } reset;

  struct KeyCheck {
    mutable bool m_result = false;
    std::string m_toCheck;
    template <typename T> void operator()(const T &t) const {
      if (this->m_result)
        return;

      for (auto &p : t) {
        this->m_result = this->m_result || (this->m_toCheck == p.first);

        if (this->m_result)
          break;
      }
    } // ()
  } keyCheck;

protected:
  void StaticSetBranches(const std::string &analysisType, Derived *d) {
    addBranch.m_tree = d;
    addBranch.m_analysisType = analysisType;
    this->for_each(addBranch);
  } // StaticSetBranches

  void StaticReset() { this->for_each(reset); }

public:
  class StaticMVATree_Proxy {
    StaticMVATree &m_tree;
    std::string m_key;

  public:
    StaticMVATree_Proxy(StaticMVATree &tree, const std::string &key)
        : m_tree(tree), m_key(key) {}

    template <class Ref> operator Ref() { return m_tree.get<Ref>(m_key); }

    template <class Ref> StaticMVATree &operator=(const Ref &val) {
      m_tree.set(m_key, val);
      return m_tree;
    }
  };

  virtual ~StaticMVATree() {}
  StaticMVATree() : m_variableMaps(), addBranch(), reset(), keyCheck() {}

  // basic things
  template <class Ref>
  auto access(const std::string &name)
      -> decltype(std::get<Analysis::type_index<0, Ref, Types...>::type::index>(
          m_variableMaps)[name]) {
    return std::get<Analysis::type_index<0, Ref, Types...>::type::index>(
        m_variableMaps)[name];
  }

  template <class Ref> Ref get(const std::string &name) {
    if (!this->exists<Ref>(name))
      Fatal("StaticMVATree::get()", "key (%s) does not exist", name.c_str());
    // val = std::get<0>(std::get<Analysis::type_index<0, Ref, Types
    // ...>::type::index>(m_variableMaps)[name]);
    return std::get<0>(this->access<Ref>(name));
  }

  template <class Ref> void get(const std::string &name, Ref &val) {
    // val = std::get<0>(std::get<Analysis::type_index<0, Ref, Types
    // ...>::type::index>(m_variableMaps)[name]);
    val = this->get<Ref>(name);
  }

  template <class Ref> void set(const std::string &name, const Ref &val) {
    // std::get<0>(std::get<Analysis::type_index<0, Ref, Types
    // ...>::type::index>(m_variableMaps)[name]) = val;
    std::get<0>(this->access<Ref>(name)) = val;
  }

  template <class Func> void for_each(const Func &f) {
    StaticMVATree_internal::for_each_MVATree(m_variableMaps, f);
  }

  template <class Ref> bool exists(const std::string &key) {
    return std::get<Analysis::type_index<0, Ref, Types...>::type::index>(
               m_variableMaps)
        .count(key);
  }

  bool exists(const std::string &key) {
    keyCheck.m_result = false;
    keyCheck.m_toCheck = key;
    this->for_each(keyCheck);
    return keyCheck.m_result;
  } // exists

  StaticMVATree_Proxy operator[](const std::string &key) {
    return StaticMVATree_Proxy(*this, key);
  }

  // real usage
  template <class T>
  void
  SetBranchAndValue(const std::string &name, const T &val, const T &default_val,
                    const std::string &analysis_type = "StaticMVATree::ALL") {
    if (this->exists<T>(name)) {
      // if exists, just update value. default_val and analysis_type have no
      // reason to change (?)
      std::get<0>(this->access<T>(name)) = val;
    } else {
      this->access<T>(name) =
          std::make_tuple(val, default_val, analysis_type, false);
    }
  }
};

#endif // ifndef CxAODReader_StaticMVATree_H
