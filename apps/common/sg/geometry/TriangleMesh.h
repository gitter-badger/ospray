// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "sg/common/Node.h"
#include "sg/geometry/Geometry.h"
#include "sg/common/Data.h"
#include "sg/common/World.h"
#include "common/sg/SceneGraph.h"

namespace ospray {
  namespace sg {

    struct Importer : public sg::World {
      Importer() : World() {}

      virtual void init() override
      {
        Renderable::init();
        add(createNode("fileName", "string"));
      //         // World::preCommit(ctx);
      // ospcommon::FileName file = getChild("fileName")->getValue<std::string>();
      // // if (file.str() == loadedFileName)
      //   // return;
      // if (loadedFileName != "")
      //   return; //TODO: support dynamic re-loading, need to clear children first
      // loadedFileName = "";
      // if (file.ext() == "obj")
      //   sg::importOBJ(std::static_pointer_cast<sg::World>(shared_from_this()), file);
      // loadedFileName = file.str();
      // // traverse(ctx, "verify");
      // traverse(ctx, "print");
      }
      virtual void setChildrenModified(TimeStamp t) override
      {
      Node::setChildrenModified(t);
        std::cout << "wakka modified\n" << std::endl;
        // ospcommon::FileName file(getChild("fileName")->getValue<std::string>());
        ospcommon::FileName file("/bertha/teapot.obj");
      if (file.str() == loadedFileName)
        return;
        std::cout << "attempting importing file: " << file.str() << std::endl;
      if (loadedFileName != "" || file.str() == "")
        return; //TODO: support dynamic re-loading, need to clear children first
      loadedFileName = "";
      if (file.ext() == "obj")
      {
        std::cout << "importing file: " << file.str() << std::endl;
        sg::importOBJ(std::static_pointer_cast<sg::World>(shared_from_this()), file);
      }
      loadedFileName = file.str();
      }
       virtual void preCommit(RenderContext &ctx) override;

       std::string loadedFileName;
    };

    /*! A Simple Triangle Mesh that stores vertex, normal, texcoord,
        and vertex color in separate arrays */
    struct TriangleMesh : public sg::Geometry {

      //! constructor
      TriangleMesh() : Geometry("trianglemesh"), ospGeometry(NULL) {}

      virtual void init() override
      {
        // add(createNode("vertex", "DataBuffer"));
        // add(createNode("normal", "DataBuffer"));
        // add(createNode("color", "DataBuffer"));
        // add(createNode("texcoord", "DataBuffer"));
        // add(createNode("index", "DataBuffer"));
        add(createNode("material", "Material"));
        add(createNode("visible", "bool", true));
        add(createNode("position", "vec3f"));
        add(createNode("rotation", "vec3f", vec3f(0), NodeFlags::required | NodeFlags::valid_min_max | NodeFlags::gui_slider));
        getChild("rotation")->setMinMax(-vec3f(2*3.15f),vec3f(2*3.15f));
        add(createNode("scale", "vec3f", vec3f(1.f)));
      };
      
      /*! \brief returns a std::string with the c++ name of this class */
      virtual    std::string toString() const { return "ospray::sg::Geometry"; }

      //! return bounding box of all primitives
      virtual box3f getBounds();

      virtual void preRender(RenderContext &ctx) override;

      virtual void postCommit(RenderContext &ctx) override;

      /*! 'render' the nodes */
      virtual void render(RenderContext &ctx);

      //! \brief Initialize this node's value from given XML node 
      /*!
        \detailed This allows a plug-and-play concept where a XML
        file can specify all kind of nodes wihout needing to know
        their actual types: The XML parser only needs to be able to
        create a proper C++ instance of the given node type (the
        OSP_REGISTER_SG_NODE() macro will allow it to do so), and can
        tell the node to parse itself from the given XML content and
        XML children 
        
        \param node The XML node specifying this node's fields

        \param binBasePtr A pointer to an accompanying binary file (if
        existant) that contains additional binary data that the xml
        node fields may point into
      */
      virtual void setFromXML(const xml::Node *const node, const unsigned char *binBasePtr);

      OSPGeometry         ospGeometry;
      OSPGeometry         ospGeometryInstance;
      OSPModel ospModel;
      
      // to allow memory-mapping triangle arrays (or in general,
      // sharing data with an application) we use data arrays, not std::vector's

      //! vertex (position) array
      std::shared_ptr<DataBuffer> vertex;
      
      //! vertex normal array. empty means 'not present'
      std::shared_ptr<DataBuffer> normal;
      
      //! vertex color array. empty means 'not present'
      std::shared_ptr<DataBuffer> color;

      //! vertex texture coordinate array. empty means 'not present'
      std::shared_ptr<DataBuffer> texcoord;

      //! triangle indices
      std::shared_ptr<DataBuffer> index;
    };


    /*! A special triangle mesh that allows per-triangle materials */
    struct PTMTriangleMesh : public sg::Geometry {

      /*! triangle with per-triangle material ID */
      struct Triangle {
        uint32_t vtxID[3], materialID;
      };

      //! constructor
      PTMTriangleMesh() : Geometry("trianglemesh"), ospGeometry(NULL) {};
      
      // return bounding box of all primitives
      virtual box3f getBounds();

      /*! 'render' the nodes */
      virtual void render(RenderContext &ctx);

      OSPGeometry         ospGeometry;

      /*! \brief "material list" for this trianglemesh 
        
        If non-empty, the 'Triangle::materialID' indexes into this
        list; if empty, all trianlges should use the
        Geometry::material no matter what Triangle::materialID is set
       */
      std::vector<std::shared_ptr<sg::Material> > materialList;
      std::vector<uint32_t> materialIDs;

      // to allow memory-mapping triangle arrays (or in general,
      // sharing data with an application) we use data arrays, not std::vector's

      //! vertex (position) array
      std::shared_ptr<DataBuffer> vertex;
      
      //! vertex normal array. empty means 'not present'
      std::shared_ptr<DataBuffer> normal;
      
      //! vertex color array. empty means 'not present'
      std::shared_ptr<DataBuffer> color;

      //! vertex texture coordinate array. empty means 'not present'
      std::shared_ptr<DataBuffer> texcoord;

      //! triangle indices
      std::shared_ptr<DataBuffer> index;
      
      //! material IDs
      OSPData primMatIDs;
   };

  } // ::ospray::sg
} // ::ospray


