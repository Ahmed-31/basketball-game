#include "model.h"
#pragma once
class ModelGroup : Model {
private:
public:
    vector<Model> childModels;
    ModelTransform transform ;
    void loadChildModel(vector<string> filenames) {
        for (const auto& filename : filenames) {
        Model childModel;
        childModel.loadObjFile(filename);
        childModels.push_back(childModel);
        }
    }
    void drawShape() {
        glTranslatef(transform.position.x, transform.position.y, transform.position.z);
        glRotatef(transform.rotation.x, 1, 0, 0);
        glRotatef(transform.rotation.y, 0, 1, 0);
        glRotatef(transform.rotation.z, 0, 0, 1);
        glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
        for (auto& childModel : childModels) {
            glPushMatrix();
            glTranslatef(childModel.transform.position.x, childModel.transform.position.y, childModel.transform.position.z);
            glRotatef(childModel.transform.rotation.x, 1, 0, 0);
            glRotatef(childModel.transform.rotation.y, 0, 1, 0);
            glRotatef(childModel.transform.rotation.z, 0, 0, 1);
            glScalef(childModel.transform.scale.x, childModel.transform.scale.y, childModel.transform.scale.z);
            childModel.drawShape();
            glPopMatrix();
        }
    }
};

