//------------------------------------------------------------------------------------//
//                                                                                    //
//               _________        .__  __    __                                       //
//               \_   ___ \_______|__|/  |__/  |_  ___________                        //
//               /    \  \/\_  __ \  \   __\   __\/ __ \_  __ \                       //
//               \     \____|  | \/  ||  |  |  | \  ___/|  | \/                       //
//                \______  /|__|  |__||__|  |__|  \___  >__|                          //
//                       \/                           \/                              //
//                                                                                    //
//    Critter is provided under the MIT License(MIT)                                  //
//    Critter uses portions of other open source software.                            //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2015 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//
#ifndef INCLUDED_CRT_XFORM
#define INCLUDED_CRT_XFORM

#include <CtrPlatform.h>
#include <CtrRenderNode.h>
#include <CtrTypedProperty.h>
#include <CtrIDevice.h>

namespace Ctr
{
class TransformProperty;
class TransformNode;

class TransformNode : public Ctr::RenderNode
{
  public:
    TransformNode (Ctr::IDevice* device);
    virtual ~TransformNode ();

    Ctr::Vector3f               worldTranslation() const;

    const Ctr::Matrix44f&       worldTransform() const;
    const Ctr::Vector3f&        translation() const;
    const Ctr::Vector3f&        rotation() const;
    const Ctr::Vector3f&        scale () const;
    
    TransformProperty*         worldTransformProperty();
    VectorProperty*            translationProperty();
    VectorProperty*            rotationProperty();
    VectorProperty*            scaleProperty();

    const TransformProperty*   worldTransformProperty() const;
    const VectorProperty*      translationProperty() const;
    const VectorProperty*      rotationProperty() const;
    const VectorProperty*      scaleProperty() const;

    void                       setParent(Ctr::TransformNode* parent);
    Ctr::TransformNode*         parent();
    Ctr::TransformNode*         child(const std::string& name);
    const std::vector <Ctr::TransformNode*> &    children() const;

    void                       cacheLastWorldTransform() const;
    MatrixProperty*            lastWorldTransformProperty() const;

  protected:
    TransformProperty*         _worldTransformProperty;
    VectorProperty*            _translationProperty;
    VectorProperty*            _rotationProperty;
    VectorProperty*            _scaleProperty;
    DeviceProperty*            _deviceProperty;
    std::vector <Ctr::TransformNode*> _entities;
    Ctr::TransformNode*       _parent;
    MatrixProperty*                 _lastWorldTransformProperty;
};

}

#endif