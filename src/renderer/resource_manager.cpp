#include "resource_manager.hpp"

#include "shaders/common.vs.hpp"
#include "shaders/phong.fs.hpp"
#include "shaders/shadows.fs.hpp"
#include "shaders/constant.fs.hpp"
#include "shaders/per_vertex.fs.hpp"

namespace clsim { namespace impl {


const char *ShaderResourceManager::fetch(const std::string &name) {
    auto it = instance_.sources_.find(name) ;
    if ( it != instance_.sources_.end() ) return it->second.c_str() ;
    else return nullptr ;
}

ShaderResourceManager::ShaderResourceManager() {
    addSource("vertex_shader", vertex_shader_code) ;
    addSource("phong_fragment_shader_vars", phong_fragment_shader_vars) ;
    addSource("phong_fragment_shader_common", phong_fragment_shader_common) ;
    addSource("phong_fragment_shader", phong_fragment_shader) ;
    addSource("shadows_fragment_shader", shadows_fragment_shader);
    addSource("constant_fragment_shader_vars", constant_fragment_shader_vars);
    addSource("constant_fragment_shader", constant_fragment_shader) ;
    addSource("per_vertex_color_fragment_shader", per_vertex_color_fragment_shader);
}

void ShaderResourceManager::addSource(const char * name, const char *src) {
    sources_.emplace(name, src) ;
}

ShaderResourceManager ShaderResourceManager::instance_ ;

}}
