//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
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

#include <IblApplicationHUD.h>
#include <IblApplication.h>
#include <CtrCamera.h>
#include <CtrScene.h>
#include <CtrEntity.h>
#include <CtrMaterial.h>
#include <CtrMesh.h>
#include <CtrPostEffect.h>
#include <CtrPostEffectsMgr.h>
#include <CtrEntity.h>
#include <CtrMaterial.h>
#include <CtrIBLProbe.h>
#include <CtrBrdf.h>
#include <CtrTextureMgr.h>
#include <Ctrimgui.h>
#include <CommDlg.h>

namespace Ctr
{
#define MAX_FILE_PATH_NAME 1024

IBLApplicationHUD* IBLApplicationHUD::_applicationHud = 0;

BOOL selectFilenameLoad(LPWSTR filename, 
    LPWSTR filter)
{
    OPENFILENAME ofn;       // common dialog box structure
    WCHAR dirName[MAX_FILE_PATH_NAME];

    BOOL fileOpenStatus;

    // Initialize OPENFILENAME structure
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_FILE_PATH_NAME;
    ofn.lpstrFilter = filter; 
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrFileTitle = L"Open File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    _wgetcwd(dirName, MAX_FILE_PATH_NAME);
    fileOpenStatus = ::GetOpenFileName(&ofn);
    _wchdir(dirName);

    return fileOpenStatus;
}

BOOL selectFilenameSave(WindowHandle windowHandle, 
    LPWSTR filename, 
    LPWSTR filter)
{
    OPENFILENAME ofn = { sizeof(OPENFILENAME), windowHandle, NULL,
        filter, NULL, 0, 1,
        filename, MAX_FILE_PATH_NAME, NULL, 0,
        NULL, L"Save As", NULL,
        0, 0, NULL,
        0, NULL, NULL };
    BOOL fileSaveStatus;
    WCHAR dirName[MAX_FILE_PATH_NAME];

    _wgetcwd(dirName, MAX_FILE_PATH_NAME);
    fileSaveStatus = ::GetSaveFileName(&ofn);
    _wchdir(dirName);

    return fileSaveStatus;
}

IBLApplicationHUD::IBLApplicationHUD (Ctr::IBLApplication* application,
                                Ctr::IDevice* device,
                                Ctr::InputState* inputState,
                                Ctr::Scene* scene) : 
RenderHUD (application, device, inputState),
_scene (scene),
_controlsVisible (false),
_munkyfunTexture(nullptr),
_gameTexture(nullptr),
_showRendering(false),
_renderingEnabled(true),
_showAbout(false),
_aboutEnabled(true),
_showEnvironment(true),
_environmentEnabled(true),
_showBrdf(true),
_brdfEnabled(true),
_showFiltering(true),
_filteringEnabled(true),
_scrollArea(0),
_iblApplication(application)
{
    _applicationHud = this;
    Ctr::Camera* camera = _scene->camera();

    _controlsVisible = true;
    setUIVisible(true);

    _munkyfunTexture = device->textureMgr()->loadTexture("data/textures/BbTitles/Title.dds");
    _gameTexture = device->textureMgr()->loadTexture("data/textures/BbTitles/gametextures_logo.dds");
}

void
IBLApplicationHUD::setupMeshUI()
{
}

void
IBLApplicationHUD::cleanupMeshUI()
{
}

void
IBLApplicationHUD::showApplicationUI()
{

}

IBLApplicationHUD::~IBLApplicationHUD()
{
}

bool
IBLApplicationHUD::create()
{
    return RenderHUD::create();
}

bool
IBLApplicationHUD::update(double elapsedTime)
{
    if (_inputState->getKeyState(DIK_F1))
    {
        _controlsVisible = true;
        setUIVisible (true);
    }
    if (_inputState->getKeyState(DIK_F2))
    {
        _controlsVisible = false;
        setUIVisible (false);
    }

    if (RenderHUD::update(elapsedTime))
    {
        return true;
    }
    return false;
}

uint32_t idFromVals(int32_t value, const ImguiEnumVal* values, uint32_t valueCount)
{
    for (uint32_t valueId = 0; valueId < valueCount; valueId++)
    {
        if (values[valueId].value == value)
        {
            return valueId;
        }
    }

    LOG("Program error: No such value " << value << " in enum representation.");
    return 0;
}

template <class T>
void
chooseForProperty(T* property)
{
    const ImguiEnumVal* enumVals = property->tweakFlags()->enumType->enumValues();
    uint32_t currentValue = property->get();
    uint32_t enumCount = property->tweakFlags()->enumType->enumCount();
    uint32_t currentId = idFromVals(currentValue, enumVals, enumCount);

    uint32_t newId = imguiChooseFromArrayInstead(currentId, enumVals, enumCount);
    if (newId != currentId)
    {
        property->set(enumVals[newId].value);
    }
}

template <class T>
void
chooseForProperties(T** properties, uint32_t numProperties)
{
    const ImguiEnumVal* enumVals = properties[0]->tweakFlags()->enumType->enumValues();
    uint32_t currentValue = properties[0]->get();
    uint32_t enumCount = properties[0]->tweakFlags()->enumType->enumCount();
    uint32_t currentId = idFromVals(currentValue, enumVals, enumCount);

    uint32_t newId = imguiChooseFromArrayInstead(currentId, enumVals, enumCount);
    if (newId != currentId)
    {
        for (uint32_t propertyId = 0; propertyId < numProperties; propertyId++)
            properties[propertyId]->set(enumVals[newId].value);
    }
}

static void imguiRegion(const char* title, const char* strRight, bool& flag, bool enabled = true)
{
    if (imguiCollapse(title, strRight, flag, enabled))
    {
        flag = !flag;
    }
}

void imguiRegionBorder(const char* title, const char* strRight, bool& flag, bool enabled = true)
{
    imguiSeparatorLine(1);
    imguiRegion(title, strRight, flag, enabled);
    imguiSeparatorLine(1);
    imguiSeparator(4);
}

template <class T, class S>
void
imguiPropertySlider(const char* title, T* property, float min, float max, S step, bool enabled = true)
{
    float displayGamma = float(property->get());
    if (imguiSlider(title, displayGamma, min, max, float(step), enabled))
    {
        property->set(S(displayGamma));
    }
}

template <class T, typename S>
void
imguiPropertiesSlider(const char* title, T** properties, uint32_t propertyCount, float min, float max, S step, bool enabled = true)
{
    float value = float(properties[0]->get());
    if (imguiSlider(title, value, min, max, float(step), enabled))
    {
        for (uint32_t propertyId = 0; propertyId < propertyCount; propertyId++)
            properties[propertyId]->set(S(value));
    }
}

template <class T>
void
imguiSelectionSliderForEnumProperty(const char* title, T* property)
{
    const ImguiEnumVal* enumVals = property->tweakFlags()->enumType->enumValues();
    uint32_t currentValue = uint32_t(property->get());
    uint32_t enumCount = property->tweakFlags()->enumType->enumCount();
    uint32_t currentId = uint32_t(idFromVals(currentValue, enumVals, uint32_t(enumCount)));

    float minValue = float(0);
    float maxValue = float(enumCount-1);

    std::vector <const char*> labels;
    labels.reserve(enumCount);
    for (uint32_t labelId = 0; labelId < enumCount; labelId++)
        labels.push_back(enumVals[labelId].label);
    imguiLabel(title);

    const uint8_t tab = imguiTabsForEnum(uint8_t(currentId), true, ImguiAlign::CenterIndented, 16, 2, enumCount, enumVals);
    if (uint32_t(tab) != currentId)
    {
        property->set(enumVals[tab].value);
    }
}

template <class T>
void
imguiSelectionSliderForPixelFormatProperty(const char* title, T* property)
{
    const ImguiEnumVal* enumVals = property->tweakFlags()->enumType->enumValues();
    uint32_t currentValue = uint32_t(property->get());
    uint32_t enumCount = property->tweakFlags()->enumType->enumCount();
    uint32_t currentId = uint32_t(idFromVals(currentValue, enumVals, uint32_t(enumCount)));

    float minValue = float(0);
    float maxValue = float(enumCount - 1);

    std::vector <const char*> labels;
    labels.reserve(enumCount);
    for (uint32_t labelId = 0; labelId < enumCount; labelId++)
        labels.push_back(enumVals[labelId].label);
    imguiLabel(title);

    const uint8_t tab = imguiTabsForEnum(uint8_t(currentId), true, ImguiAlign::CenterIndented, 16, 2, enumCount, enumVals);
    if (uint32_t(tab) != currentId)
    {
        property->set((PixelFormat)enumVals[tab].value);
    }
}

template <class T>
void
imguiColorWheelForProperty(const char* title, T* property)
{
    // If I am drawing it, it is activated for now.
    const static bool activated = true;
    const T& color = property->get();
    imguiColorWheel("Diffuse color:", &color.x, activated);
}

template <class T>
void
imguiColorWheelForProperties(const char* title, T* properties, uint32_t propertyCount)
{
    // If I am drawing it, it is activated for now.
    static bool activated = true;
    float* color = &properties[0]->get().x;
    // TODO: Needs fixing. Performance drag due to floating point compare and no
    // no invalidation mechanism. Add invalidation mechanism (bool return from imguiColorWheel if value changed).
    imguiColorWheel(title, color, activated);

    for (uint32_t propertyId = 1; propertyId < propertyCount; propertyId++)
    {
        properties[propertyId]->set(properties[0]->get());
    }
}

template <class T, typename S>
void
imguiSliderForPropertyChannel(const char* title, T* property, uint32_t channel, float min, float max, float step)
{
    T& channels = property->get();
    float value = float(channels[channel]);

    if (imguiSlider(title, value, min, max, float(step), enabled))
    {
        // Could do with some validation.
        channels[channelId] = channels;
        property->set(S(value));
    }
}

template <class T>
void
imguiSliderForPropertiesChannel(const char* title, T** properties, uint32_t channel, uint32_t propertyCount, float min, float max, float step)
{
    float* channels = &properties[0]->get().x;
    float value = float(channels[channel]);

    // If we draw it, it is activated.
    static bool activated = true;

    if (imguiSlider(title, value, min, max, float(step), activated))
    {
        // Yay, reference return... Invalidation doesn't work properly for this though.
        properties[0]->get()[channel] = value;

        // For all variants, set.
        for (uint32_t propertyId = 1; propertyId < propertyCount; propertyId++)
            properties[propertyId]->set(properties[0]->get());
    }
}

void
IBLApplicationHUD::initModel(Ctr::Entity* entity)
{
    _entity = _iblApplication->loadEntity();
}

void
IBLApplicationHUD::render(const Ctr::Camera* camera)
{
    int32_t width = _deviceInterface->backbuffer()->width();
    int32_t height = _deviceInterface->backbuffer()->height();

    if (_uiVisible)
    {
        Ctr::Vector2i imguiWindowMin(10, 10);
        Ctr::Vector2i imguiWindowMax = imguiWindowMin + Ctr::Vector2i(width / 5, height - 50);
        Ctr::Region2i imguiWindowBounds(imguiWindowMin, imguiWindowMax);
        _inputState->setHasGUIFocus(imguiWindowBounds.intersects(Ctr::Vector2i(_inputState->_cursorPositionX, _inputState->_cursorPositionY)));

        imguiBeginFrame(_inputState, _inputState->_cursorPositionX, _inputState->_cursorPositionY, _inputState->leftMouseDown() ? IMGUI_MBUT_LEFT : 0 | _inputState->rightMouseDown() ? IMGUI_MBUT_RIGHT : 0, int32_t(_inputState->_z / 25.5), width, height);

        imguiBeginScrollArea("IBLBaker", imguiWindowBounds.minExtent.x, imguiWindowBounds.minExtent.y,
                             imguiWindowBounds.maxExtent.x, imguiWindowBounds.maxExtent.y, &_scrollArea);
        imguiSeparator();

        if (imguiButton("Load Environment"))
        {
            LOG("Loading an environment");
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR * filter = L"All\0*.*\0Text\0*.TXT\0";

            if (selectFilenameLoad(selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());

                _iblApplication->loadEnvironment(std::string(filePathName.c_str()));
            }
        }
        if (imguiButton("Load Model"))
        {
            LOG("Loading an Model");
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR* filter = L"All\0*.*\0Text\0*.TXT\0";

            if (selectFilenameLoad(selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());

                _iblApplication->loadModel(std::string(filePathName.c_str()));
            }
        }
        if (imguiButton("Save Environment"))
        {
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR * filter = L"DirectDraw Surfaces(*.dds)\0*.dds\0\0";
            if (Ctr::selectFilenameSave(_application->window()->windowHandle(),
                selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());

                _iblApplication->saveImages(filePathName.c_str());
            }
        }
        if (imguiButton("Compute"))
        {
            _iblApplication->compute();
        }
        if (imguiButton("Cancel Compute"))
        {
            _iblApplication->cancel();
        }

        imguiRegionBorder("About:", NULL, _showAbout, _aboutEnabled);
        if (_showAbout)
        {
            imguiIndent();
            imguiLabel("Matt Davidson (2015)");
            imguiLabel("Thankyou to:");
            imguiLabel("www.munkyfun.com");
            imguiImage(const_cast<ITexture*>(_munkyfunTexture), 0, 256, 128, ImguiAlign::Center);
            imguiLabel("www.gametextures.com");
            imguiImage(const_cast<ITexture*>(_gameTexture), 0, 256, 128, ImguiAlign::Center);

            imguiUnindent();
        }

        imguiRegionBorder("Rendering:", NULL, _showRendering, _renderingEnabled);
        if (_showRendering)
        {
            imguiIndent();
            imguiPropertySlider("Exposure", _scene->camera()->exposureProperty(), 0.0f, 10.0f, 0.01f);
            imguiPropertySlider("Display Gamma", _scene->camera()->gammaProperty(), 0.0f, 5.0f, 0.01f);

            imguiSelectionSliderForEnumProperty("View Model", _iblApplication->modelVisualizationProperty());
            _iblApplication->syncVisualization();

            _entity = _iblApplication->visualizedEntity();
            if (_iblApplication->modelVisualizationProperty()->get() == Ctr::IBLApplication::ShaderBallModel)
                _entity = _iblApplication->shaderBallEntity();


            std::vector<Ctr::IntProperty*> debugTermProperties;
            std::vector<Ctr::IntProperty*> specularWorkflowProperties;
            std::vector<Ctr::FloatProperty*> roughnessScaleProperties;
            std::vector<Ctr::FloatProperty*> specularIntensityProperties;
            std::vector<Ctr::Vector4fProperty*> userAlbedoProperties;
            std::vector<Ctr::Vector4fProperty*> userRMProperties;

            const std::vector<Mesh*>& meshes = _entity->meshes();

            specularWorkflowProperties.push_back(_iblApplication->specularWorkflowProperty());
            debugTermProperties.push_back(_iblApplication->debugTermProperty());

            if (_iblApplication->modelVisualizationProperty()->get() == Ctr::IBLApplication::ShaderBallModel)
            {
                for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
                {

                    // ArseImp doesn't load mat or mesh names (or I didn't turn on a preprocessor define to do so).
                    // Need to rfind on Albedo for brick.
                    // *Note to self, Port ArseImp Fbx implementation to standalone lightweight library*
                    //
                    Material* currentMaterial = (*meshIt)->material();
                    if (currentMaterial->name().rfind("Brick") != std::string::npos)
                    { 
                        userAlbedoProperties.push_back(currentMaterial->userAlbedoProperty());
                        userRMProperties.push_back(currentMaterial->userRMProperty());
                    }
                }
            }
            else
            {            
                for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
                {
                    Material* currentMaterial = (*meshIt)->material();
                    userAlbedoProperties.push_back(currentMaterial->userAlbedoProperty());
                    userRMProperties.push_back(currentMaterial->userRMProperty());
                }
            }

            for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
            {
                Material* currentMaterial = (*meshIt)->material();
                Mesh* currentMesh = (*meshIt);
                debugTermProperties.push_back(currentMaterial->debugTermProperty());
                specularWorkflowProperties.push_back(currentMaterial->specularWorkflowProperty());
                specularIntensityProperties.push_back(currentMaterial->specularIntensityProperty());
                roughnessScaleProperties.push_back(currentMaterial->roughnessScaleProperty());
            }

            imguiSliderForPropertiesChannel("User Albedo", &userAlbedoProperties[0], 3, (uint32_t)userAlbedoProperties.size(), 0.0f, 1.0f, 0.05f);
            imguiColorWheelForProperties("User Albedo Term", &userAlbedoProperties[0], (uint32_t)userAlbedoProperties.size());

            // These are in whatever working space you have selected.
            imguiSliderForPropertiesChannel("User Gloss/Roughness", &userRMProperties[0], 0, (uint32_t)userRMProperties.size(), 0.0f, 1.0f, 0.05f);
            imguiSliderForPropertiesChannel("User Weight R/G", &userRMProperties[0], 1, (uint32_t)userRMProperties.size(), 0.0f, 1.0f, 0.05f);
            imguiSliderForPropertiesChannel("User Metalness", &userRMProperties[0], 2, (uint32_t)userRMProperties.size(), 0.0f, 1.0f, 0.05f);
            imguiSliderForPropertiesChannel("User Weight M", &userRMProperties[0], 3, (uint32_t)userRMProperties.size(), 0.0f, 1.0f, 0.05f);

            uint32_t meshCount = uint32_t(meshes.size());
            imguiPropertiesSlider("Roughness Scale", &roughnessScaleProperties[0], (uint32_t)roughnessScaleProperties.size(), 0.0f, 1.0f, 0.05f);
            imguiPropertiesSlider("Specular Intensity", &specularIntensityProperties[0], (uint32_t)specularIntensityProperties.size(), 0.0f, 6.0f, 0.05f);
            imguiLabel("Specular Workflow");
            chooseForProperties(&specularWorkflowProperties[0], (uint32_t)specularWorkflowProperties.size());

            imguiLabel("Debug Channel");
            chooseForProperties(&debugTermProperties[0], (uint32_t)debugTermProperties.size());

            imguiUnindent();
        }


        //static float blah[3] = {0.0, 1.0, 1.0};
        //static bool activated = true;
        //imguiColorWheel("Diffuse color:", blah, activated);

        imguiRegionBorder("Filtering:", NULL, _showFiltering, _filteringEnabled);
        if (_showFiltering)
        {
            imguiIndent();

            FloatProperty* inputGammas[2] = { 
                _iblApplication->sphereEntity()->mesh(0)->material()->textureGammaProperty(),
                _iblApplication->iblSphereEntity()->mesh(0)->material()->textureGammaProperty()
            };

            imguiPropertiesSlider("Input Gamma", &inputGammas[0], 2, 0.0f, 5.0f, 0.01f);
            imguiPropertySlider("Environment Scale", _scene->probes()[0]->environmentScaleProperty(), 0.0f, 10.0f, 0.1f);

            // Lock sample counts for now.
            IntProperty* inputSamples[2] = {
                _scene->probes()[0]->sampleCountProperty(),
                _scene->probes()[0]->samplesPerFrameProperty()
            };

            imguiPropertiesSlider("Sample Count", &inputSamples[0], 2, 0.0f, 2048.0f, 1);
            imguiPropertySlider("Mip Drop", _scene->probes()[0]->mipDropProperty(), 0.0f, _scene->probes()[0]->specularCubeMap()->resource()->mipLevels() - 1.0f, 1);
            imguiPropertySlider("Saturation", _scene->probes()[0]->iblSaturationProperty(), 0.0f, 1.0f, 0.05f);
            //imguiPropertySlider("Contrast", _scene->probes()[0]->iblContrastProperty(), 0.0f, 1.0f, 0.05f);
            imguiPropertySlider("Hue", _scene->probes()[0]->iblHueProperty(), 0.0f, 1.0f, 0.05f);
            imguiPropertySlider("Max Pixel R", _scene->probes()[0]->maxPixelRProperty(), 0.0f, 1000.0f, 1.0f, false);
            imguiPropertySlider("Max Pixel G", _scene->probes()[0]->maxPixelGProperty(), 0.0f, 1000.0f, 1.0f, false);
            imguiPropertySlider("Max Pixel B", _scene->probes()[0]->maxPixelBProperty(), 0.0f, 1000.0f, 1.0f, false);

            imguiSelectionSliderForPixelFormatProperty("Environment Format", _scene->probes()[0]->hdrPixelFormatProperty());
            imguiSelectionSliderForEnumProperty("Source Resolution", _scene->probes()[0]->sourceResolutionProperty());
            imguiSelectionSliderForEnumProperty("Specular Resolution", _scene->probes()[0]->specularResolutionProperty());
            imguiSelectionSliderForEnumProperty("Diffuse Resolution", _scene->probes()[0]->diffuseResolutionProperty());

            imguiUnindent();
        }

        imguiRegionBorder("Brdf:", NULL, _showBrdf, _brdfEnabled);
        if (_showBrdf)
        {
            imguiIndent();

            chooseForProperty(_scene->activeBrdfProperty());

            imguiImage(const_cast<ITexture*>(_scene->activeBrdf()->brdfLut()), 0, 128, 128, ImguiAlign::Center);

            imguiUnindent();
        }

        imguiRegionBorder("Environment:", NULL, _showEnvironment, _environmentEnabled);
        if (_showEnvironment)
        {
            imguiIndent();
            static float _lod = 0.0f;
            static bool _crossCubemapPreview = false;
            if (imguiCube(_scene->probes()[0]->environmentCubeMap(), _lod, _crossCubemapPreview))
            {
                _crossCubemapPreview = !_crossCubemapPreview;
            }

            imguiLabel("Specular IBL:");
            static float _specularEnvLod = 0;
            float maxSpecularMipLevels = _scene->probes()[0]->specularCubeMap()->resource()->mipLevels() - 1.0f - _scene->probes()[0]->mipDropProperty()->get();
            if (_specularEnvLod > maxSpecularMipLevels)
                maxSpecularMipLevels = maxSpecularMipLevels;
            imguiSlider("IBL LOD", _specularEnvLod, 0.0f, maxSpecularMipLevels, 0.15f);
        
            static bool _specularCrossCubemapPreview = false;
            if (imguiCube(_scene->probes()[0]->specularCubeMap(), _specularEnvLod, _specularCrossCubemapPreview))
            {
                _specularCrossCubemapPreview = !_specularCrossCubemapPreview;
            }

            imguiLabel("Irradiance IBL:");
            static float _diffuseEnvLod = 0.0f;
            static bool _diffuseCrossCubemapPreview = false;
            if (imguiCube(_scene->probes()[0]->diffuseCubeMap(), _diffuseEnvLod, _diffuseCrossCubemapPreview))
            {
                _diffuseCrossCubemapPreview = !_diffuseCrossCubemapPreview;
            }
            imguiUnindent();
        }
        imguiEndScrollArea();
        imguiEndFrame();
    }

    RenderHUD::render(camera);
}



}