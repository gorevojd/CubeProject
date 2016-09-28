#ifndef FBX_SDK_MODEL_LOADING_H

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
//#include <fbxsdk/scene/geometry/fbxlayer.h>

#include "../mesh_builder.h"

#include "../FileSystem/SOIL_texture_loader.h"

#include "../Rendering/materials_code.h"

glm::vec4 FbxVector4ToGLM(FbxVector4 vec){
	glm::vec4 res;
	res.x = vec.mData[0];
	res.y = vec.mData[1];
	res.z = vec.mData[2];
	res.w = vec.mData[3];
	return res;
}

glm::vec2 FbxVector2ToGLM(FbxVector2 vec){
	glm::vec2 res;
	res.x = vec.mData[0];
	res.y = vec.mData[1];
	return res;
}

GLuint LoadTextureFromFile(std::string filePath){
	//std::string filename = std::string(fileRelativePath);
	//filename = directory + '/' + filename;
	GLuint textureID;
	loaded_texture tempTex = LoadTexture(filePath.c_str());
	textureID = tempTex.texture;
	return textureID;
}

std::vector<MeshTexture> GetMaterialTextureByProperty(FbxProperty prop, Model* model, texture_type textureType){
	std::vector<MeshTexture> res;
	if (prop.IsValid()){
		int textureCount = prop.GetSrcObjectCount<FbxTexture>();
		for (int j = 0; j < textureCount; j++){
			FbxLayeredTexture* layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(j);
			if (layeredTexture){
				int nTextures = layeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < nTextures; k++){
					FbxTexture* texture = layeredTexture->GetSrcObject<FbxTexture>(k);
					if (texture){

					}
				}
			}
			else{
				FbxTexture* texture = prop.GetSrcObject<FbxTexture>(j);
				FbxFileTexture *fileTexture = FbxCast<FbxFileTexture>(texture);
				
				bool skip = false;
				for (GLuint j = 0; j < model->textures_loaded.size(); j++)
				{
					if (model->textures_loaded[j].path == std::string(fileTexture->GetFileName()))
					{
						res.push_back(model->textures_loaded[j]);
						skip = true; // A texture with the same file path has already been loaded, continue to next one. (optimization)
						break;
					}
				}
				if (!skip){
					if (texture){
						MeshTexture tex;
						tex.TextureID = LoadTextureFromFile(std::string(fileTexture->GetFileName()));
						tex.path = std::string(fileTexture->GetFileName());
						tex.type = textureType;
						model->textures_loaded.push_back(tex);
						res.push_back(tex);
					}
				}
			}
		}
	}
	return res;
}

Mesh ProcessMesh(FbxMesh* msh, Model* model){
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<MeshTexture> textures;

	for (int i = 0; i < msh->GetControlPointsCount(); i++){
		//Vertex vertex;
		//v3 pos;
		//FbxVector4 vert = msh->GetControlPointAt(i);
		//pos.x = (float)vert.mData[0];
		//pos.y = (float)vert.mData[1];
		//pos.z = (float)vert.mData[2];
		//vertex.Position = pos;
		//vertices.push_back(vertex);
	}

	int indexForIndices = 0;
	int numFaces = msh->GetPolygonCount();
	for (int polyInd = 0; polyInd < numFaces; polyInd++){

		for (int vInd = 0; vInd < msh->GetPolygonSize(polyInd); vInd++){

			int controlPointIndex = msh->GetPolygonVertex(polyInd, vInd);
			Vertex vertex;
			v3 pos;
			FbxVector4 vert = msh->GetControlPointAt(controlPointIndex);
			pos.x = (float)vert.mData[0];
			pos.y = (float)vert.mData[1];
			pos.z = (float)vert.mData[2];
			vertex.Position = pos;
			vertices.push_back(vertex);

#if 1
			FbxVector4 normalVec;
			msh->GetPolygonVertexNormal(polyInd, vInd, normalVec);
			

			v3 norm;
			norm.x = (float)normalVec.mData[0];
			norm.y = (float)normalVec.mData[1];
			norm.z = (float)normalVec.mData[2];
			vertices[indexForIndices].Normal = norm;
#endif
			
#if 1
			FbxVector2 fbxTexCoord;
			FbxStringList UVSetNameList;
			bool haveUV;
			msh->GetUVSetNames(UVSetNameList);

			msh->GetPolygonVertexUV(polyInd, vInd, UVSetNameList.GetStringAt(0), fbxTexCoord, haveUV);
			
			v2 texCoord;
			texCoord.x = static_cast<float>(fbxTexCoord.mData[0]);
			texCoord.y = static_cast<float>(fbxTexCoord.mData[1]);
			vertices[indexForIndices].TexCoords = texCoord;
#endif

			indices.push_back(indexForIndices++);
		}

		//Getting tangents
		if (msh->GetPolygonSize(polyInd) == 3){

			v3 tangent;
			v3 bitangent;

			v3 vert1 = (glm::vec3)FbxVector4ToGLM(msh->GetControlPointAt(msh->GetPolygonVertex(polyInd, 0)));
			v3 vert2 = (glm::vec3)FbxVector4ToGLM(msh->GetControlPointAt(msh->GetPolygonVertex(polyInd, 1)));
			v3 vert3 = (glm::vec3)FbxVector4ToGLM(msh->GetControlPointAt(msh->GetPolygonVertex(polyInd, 2)));

			bool haveUV0;
			bool haveUV1;
			bool haveUV2;

			FbxVector2 fbxTexCoord0;
			FbxVector2 fbxTexCoord1;
			FbxVector2 fbxTexCoord2;

			FbxStringList UVSetNameList;
			msh->GetUVSetNames(UVSetNameList);

			msh->GetPolygonVertexUV(polyInd, 0, UVSetNameList.GetStringAt(0), fbxTexCoord0, haveUV0);
			msh->GetPolygonVertexUV(polyInd, 1, UVSetNameList.GetStringAt(0), fbxTexCoord1, haveUV1);
			msh->GetPolygonVertexUV(polyInd, 2, UVSetNameList.GetStringAt(0), fbxTexCoord2, haveUV2);

			v2 uv1 = (glm::vec2)FbxVector2ToGLM(fbxTexCoord0);
			v2 uv2 = (glm::vec2)FbxVector2ToGLM(fbxTexCoord1);
			v2 uv3 = (glm::vec2)FbxVector2ToGLM(fbxTexCoord2);

			v3 edge1 = vert2 - vert1;
			v3 edge2 = vert3 - vert1;

			v2 deltaUV1 = uv2 - uv1;
			v2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent.x = f *(deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f *(deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f *(deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);

			/*vertices[msh->GetPolygonVertex(polyInd, 0)].Tangent = tangent;
			vertices[msh->GetPolygonVertex(polyInd, 1)].Tangent = tangent;
			vertices[msh->GetPolygonVertex(polyInd, 2)].Tangent = tangent;
			*/
			vertices[indexForIndices - 3].Tangent = tangent;
			vertices[indexForIndices - 2].Tangent = tangent;
			vertices[indexForIndices - 1].Tangent = tangent;
		}
	}

#if 0
	FbxGeometryElementNormal* normalElement = msh->GetElementNormal();
	if (normalElement){
		if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint){
			for (int lVertexIndex = 0; lVertexIndex < msh->GetControlPointsCount(); lVertexIndex++){
				int lNormalIndex = 0;

				if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect){
					lNormalIndex = lVertexIndex;
				}
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect){
					lNormalIndex = normalElement->GetIndexArray().GetAt(lVertexIndex);
				}

				FbxVector4 lNormal = normalElement->GetDirectArray().GetAt(lNormalIndex);
				v3 norm;
				norm.x = (float)lNormal.mData[0];
				norm.y = (float)lNormal.mData[1];
				norm.z = (float)lNormal.mData[2];
				vertices[lNormalIndex].Normal = norm;
			}
		}
		else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex){
			int lIndexByPolygonVertex = 0;
			for (int lPolygonIndex = 0; lPolygonIndex < msh->GetPolygonCount(); lPolygonIndex++)
			{
				int lPolygonSize = msh->GetPolygonSize(lPolygonIndex);
				for (int i = 0; i < lPolygonSize; i++)
				{
					int lNormalIndex = 0;
					if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect){
						lNormalIndex = lIndexByPolygonVertex;
					}

					if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect){
						lNormalIndex = normalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
					}

					FbxVector4 lNormal = normalElement->GetDirectArray().GetAt(lNormalIndex);
					v3 norm;
					norm.x = (float)lNormal.mData[0];
					norm.y = (float)lNormal.mData[1];
					norm.z = (float)lNormal.mData[2];
					int tmpInd = msh->GetPolygonVertex(lPolygonIndex, i);
					vertices[tmpInd].Normal = norm;

					lIndexByPolygonVertex++;
				}
			}
		}
	}
#endif

#if 0
	FbxStringList lUVSetNameList;
	msh->GetUVSetNames(lUVSetNameList);
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++){
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = msh->GetElementUV(lUVSetName);

		if (!lUVElement){
			continue;
		}

		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		{
			//TODO(Dima): Logging Can not load
		}

		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		const int lPolyCount = msh->GetPolygonCount();
		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint){
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex){
				const int lPolySize = msh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex){
					int lPolyVertIndex = msh->GetPolygonVertex(lPolyIndex, lVertIndex);
					
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					FbxVector2 UVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					v2 texCoord;
					texCoord.x = (float)UVValue.mData[0];
					texCoord.y = (float)UVValue.mData[1];
					vertices[lUVIndex].TexCoords = texCoord;
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex){
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex){
				const int lPolySize = msh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex){
					if (lPolyIndexCounter < lIndexCount){
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;
						//int lUVIndex = msh->GetTextureUVIndex(lPolyIndex, lVertIndex);

						FbxVector2 UVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
						v2 texCoord;
						texCoord.x = (float)UVValue.mData[0];
						texCoord.y = (float)UVValue.mData[1];
						int tmpInd = msh->GetPolygonVertex(lPolyIndex, lVertIndex);
						vertices[tmpInd].TexCoords = texCoord;

						lPolyIndexCounter++;
					}
				}
			}
		}
	}
#endif


	FbxSurfaceMaterial* mat = msh->GetNode()->GetMaterial(0);

	//sTextureChannelNames[0] = "DiffuseColor"
	//sTextureChannelNames[1] = "DiffuseFactor"
	//sTextureChannelNames[2] = "EmissiveColor"
	//sTextureChannelNames[3] = "EmissiveFactor"
	//sTextureChannelNames[4] = "AmbientColor"
	//sTextureChannelNames[5] = "AmbientFactor"
	//sTextureChannelNames[6] = "SpecularColor"
	//sTextureChannelNames[7] = "SpecularFactor"
	//sTextureChannelNames[8] = "ShininessExponent"
	//sTextureChannelNames[9] = "NormalMap"
	//sTextureChannelNames[10] = "Bump"
	//sTextureChannelNames[11] = "TransparentColor"
	//sTextureChannelNames[12] = "TransparentFactor"
	//sTextureChannelNames[13] = "ReflectionColor"
	//sTextureChannelNames[14] = "ReflectionFactor"
	//sTextureChannelNames[15] = "DisplacementColor"
	//sTextureChannelNames[16] = "VectorDisplacementColor"

	FbxProperty diffProp = mat->FindProperty(FbxLayerElement::sTextureChannelNames[0]);
	std::vector<MeshTexture> diffTexture = GetMaterialTextureByProperty(diffProp, model, DIFFUSE_TEXTURE);
	textures.insert(textures.begin(), diffTexture.begin(), diffTexture.end());

	FbxProperty specularProp = mat->FindProperty(FbxLayerElement::sTextureChannelNames[6]);
	std::vector<MeshTexture> specTexture = GetMaterialTextureByProperty(specularProp, model, SPECULAR_TEXTURE);
	textures.insert(textures.begin(), specTexture.begin(), specTexture.end());

	FbxProperty emissProp = mat->FindProperty(FbxLayerElement::sTextureChannelNames[2]);
	std::vector<MeshTexture> emissTexture = GetMaterialTextureByProperty(emissProp, model, EMISSION_TEXTURE);
	textures.insert(textures.begin(), emissTexture.begin(), emissTexture.end());

	FbxProperty normProp = mat->FindProperty(FbxLayerElement::sTextureChannelNames[9]);
	std::vector<MeshTexture> normTexture = GetMaterialTextureByProperty(normProp, model, NORMAL_MAP_TEXTURE);
	textures.insert(textures.begin(), normTexture.begin(), normTexture.end());

	//FbxProperty shineProp = mat->FindProperty(FbxLayerElement::sTextureChannelNames[8]);
	float shininess = 32.0f;
	FbxSurfacePhong* ph = FbxCast<FbxSurfacePhong>(mat);
	FbxPropertyT<FbxDouble> shineProp = ph->Shininess;
	if (shineProp.IsValid()){
		FbxDouble shin = shineProp.Get();
		shininess = shin;
	}
#if 0
	phong_material PhongMat;
	PhongMat.shininess = shininess;
	for (int v = 0; v < textures.size(); v++){
		switch (textures[v].type){
		case DIFFUSE_TEXTURE:{
			PhongMat.diffuseTexture = textures[v].TextureID;
		}break;
		case SPECULAR_TEXTURE:{
			PhongMat.specularTexture = textures[v].TextureID;
		}break;
		case NORMAL_MAP_TEXTURE:{
			PhongMat.normalMapTexture = textures[v].TextureID;
		}break;
		case EMISSION_TEXTURE:{
			PhongMat.emissionTexture = textures[v].TextureID;
		}break;
		}
	}
#endif

	return Mesh(vertices, indices, textures);
}

void ProcessNode(Model* model, FbxNode* node){

	FbxMesh* msh = node->GetMesh();
	if (msh){
		int polyVerCount = msh->GetPolygonVertexCount();
		int ctrPtsCount = msh->GetControlPointsCount();
		Mesh pushMesh = ProcessMesh(msh, model);
		model->meshes.push_back(pushMesh);
	}

	for (int i = 0; i < node->GetChildCount(); i++){
		ProcessNode(model, node->GetChild(i));
	}
}

bool RekNode(FbxNode* node){
	FbxMesh* msh = node->GetMesh();
	if (msh){
		if (!msh->IsTriangleMesh()){
			return false;
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++){
		return RekNode(node->GetChild(i));
	}
	return true;
}

bool CheckIfSceneIsTriangulated(FbxScene* scene){
	FbxNode* rootNode = scene->GetRootNode();
	return RekNode(rootNode);
}

void RekSetScale(FbxNode* node, float scale){
	
	node->LclScaling.Set(FbxDouble3(scale, scale, scale));

	int nChild = node->GetChildCount();
	for (int i = 0; i < nChild; i++){
		RekSetScale(node->GetChild(i), scale);
	}
}

inline Model FBX_SDK_LoadModel(FbxManager* sdkManager, std::string filePath, float scale){
	Model model;
	model.directory = filePath.substr(0, filePath.find_last_of('/'));
	model.localTransformation = DefaultTransform();
	TransformScale(model.localTransformation, vector3(scale));

	FbxIOSettings* iosett = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(iosett);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, true);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_MODEL, true);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, true);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
	//sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);

	FbxImporter* importer = FbxImporter::Create(sdkManager, "");
	if (!importer->Initialize(filePath.c_str(), -1, sdkManager->GetIOSettings())){
		//TODO(Dima): Logging
	}

	FbxScene* scene = FbxScene::Create(sdkManager, importer->GetName());
	importer->Import(scene);
	importer->Destroy();

	FbxGeometryConverter converter(sdkManager);
	converter.Triangulate(scene, true);
	//bool sceneIsTriangled2 = CheckIfSceneIsTriangulated(scene);

	FbxNode* rootNode = scene->GetRootNode();

	//rootNode->SetGeometricScaling(FbxNode::EPivotSet::eSourcePivot, FbxVector4(scale, scale, scale, 1.0f));
	//RekSetScale(rootNode, scale);
	if (rootNode){
		ProcessNode(&model, rootNode);
	}

	return model;
}

#define FBX_SDK_MODEL_LOADING_H
#endif