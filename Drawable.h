#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <opencv2\opencv.hpp>
using namespace std;
GLuint texlen=0;
GLuint maketex(string src)
{
    cv::Mat im,im1;
    if(!cv::haveImageReader(src))
    {
        cout<<src+":file can't be opened"<<endl;
        return 0;
    }

    im=cv::imread(src);
    cout<<src+" loaded succesfuly!"<<endl;

    cv::flip(im,im1,0);
    im=im1;
    cv::cvtColor(im,im1,cv::COLOR_BGRA2RGBA);
    im=im1;

    glGenTextures(1,&texlen);
    glBindTexture(GL_TEXTURE_2D,texlen);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,im.size().width,im.size().height,0,
        GL_RGBA,GL_UNSIGNED_BYTE,im.data);
    texlen++;
    return texlen-1;
}

void split(string s,string& l,string& r,char c)
{

    l=s;
    r="0";
    for(int i=0; i<s.length(); i++)
    {
        if(s[i]==c)
        {
            l=s.substr(0,i);
            r=s.substr(i+1,s.length()-i-1);
            break;
        }
    }
    
}
class point
{
public:
	float x,y,z;
	point(){}
	point(float gx,float gy,float gz) {
		x=gx;
		y=gy;
		z=gz;
	}
    point operator+(point b)
    {
        point res(x+b.x,y+b.y,z+b.z);
        return res;
    }
    point operator-(point b)
    {
        point res=*this;
        res.x-=b.x;
        res.y-=b.y;
        res.z-=b.z;
        return res;
    }
    point operator*(float b)
    {
        point res=*this;
        res.x*=b;
        res.y*=b;
        res.z*=b;
        return res;
    }
};
class Material
{
public:
    string name;
    bool textured=0;
    GLuint tex=0;
    point color={0,0,0};
};
class Group {
public:
    string name;
    int MaterialId=-1;
    int first_face=-1,last_face;
};
class Drawable
{
public:

	vector<point> p;
    vector<Material> material;
    vector<pair<float,float>> tp;
	vector<vector<pair<int,int>>> face;
    vector<Group> group;
    bool textured=0;
    map<string,int> loadMaterials(string filename) {
        map<string,int> res;
        string got;
        ifstream in(filename);
        if(!in)
        {
            cout<<"file :\""+filename+"\" not found"<<endl;
            return res;
        }
        string line,get="";
        int g;
        vector<pair<int,int>> words;

        while(getline(in,line))
        {
            line+=" ";
            for(int i=0,j=0; i+j<=line.length();)
            {
                if(line[i+j]==' ')
                {
                    if(j==i)
                    {
                        i++;
                        j=0;
                    }
                    else
                    {
                        words.push_back({i,j});

                        i+=j+1;
                        j=0;
                    }
                }
                else
                {
                    j++;
                }

            }

            if(words.size()==0)continue;
            else if(line.substr(words[0].first,words[0].second)=="newmtl")
            {
                g=material.size();
                res[line.substr(words[1].first,words[1].second)]=g;
                material.push_back(Material{line.substr(words[1].first,words[1].second)});
            }
            else if(line.substr(words[0].first,words[0].second)=="Kd"||line.substr(words[0].first,words[0].second)=="kd")
            {
                material[g].color=point(atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data()),
                    atof(line.substr(words[3].first,words[3].second).data()));
            }
            else if(line.substr(words[0].first,words[0].second)=="map_Kd")
            {
                material[g].textured=1;
                material[g].tex=maketex(line.substr(words[1].first,words[1].second));
            }


            words.clear();
        }

        return res;
    }
public:
    void loadFile(string fileName,point scale=point(1,1,1),point shift=point(0,0,0)) //only one file
    {
        p.clear();
        tp.clear();
        face.clear();
        map<string,int> getmat;

        ifstream in(fileName);
        if(!in)
        {
            cout<<"file :\""+fileName+"\" not found"<<endl;
            return;
        }
        string line,get="",l,r;
        int g;
        vector<pair<int,int>> words;
        
        while(getline(in,line))
        {
            line+=" ";
            for(int i=0,j=0; i+j<=line.length();)
            {
                if(line[i+j]==' ') {
                    if(j==i)
                    {
                        i++;
                        j=0;
                    }
                    else
                    {
                        words.push_back({i,j});
                        
                        i+=j+1;
                        j=0;
                    }
                }
                else
                {
                    j++;
                }

            }
            if(words.size()==0)continue;//this is important
            if(line.substr(words[0].first,words[0].second)=="v")
            {
                p.push_back(point(atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data()),
                    atof(line.substr(words[3].first,words[3].second).data())
                    ));
            }else
            if(line.substr(words[0].first,words[0].second)=="vt")
            {
                tp.push_back({atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data())
                    });

            }
            else if(line.substr(words[0].first,words[0].second)=="f")
            {
                if(group[group.size()-1].first_face==-1)
                {
                    group[group.size()-1].first_face=face.size();
                }
                else
                {
                    group[group.size()-1].last_face=face.size();
                }
                g=face.size();
                face.push_back({});
                for(int i=1; i<words.size(); i++)
                {
                    split(line.substr(words[i].first,words[i].second),l,r,'/');
                    face[g].push_back({atoi(l.data())-1,atoi(r.data())-1});
                }
            }
            else if(line.substr(words[0].first,words[0].second)=="mtllib")
            {
                getmat=loadMaterials(line.substr(words[1].first,words[1].second));
            }else if(line.substr(words[0].first,words[0].second)=="o")
            {
                group.push_back(Group{line.substr(words[1].first,words[1].second)});
            }
            else if(line.substr(words[0].first,words[0].second)=="usemtl")
            {
                group[group.size()-1].MaterialId=getmat[line.substr(words[1].first,words[1].second)];
            }
            words.clear();
        }

        for(int i=0; i<tp.size(); i++)
        {
            if(tp[i].first<0)tp[i].first++;
            if(tp[i].second<0)tp[i].second++;
        }
        ofstream of("out.txt");
        for(Group gr : group)
        {
            of<<"g "<<gr.name<<endl;
            of<<"usemtl "<<material[gr.MaterialId].name<<endl;
            of<<material[gr.MaterialId].color.x<<" "<<material[gr.MaterialId].color.y<<" "<<material[gr.MaterialId].color.z<<endl;
            of<<material[gr.MaterialId].textured<<" "<<material[gr.MaterialId].tex<<endl;

            for(int i=gr.first_face; i<=gr.last_face; i++)
            {
                of<<"f ";
                for(int j=0; j<face[i].size(); j++)
                {
                    of<<face[i][j].first+1<<"/"<<face[i][j].second+1<<" ";
                }
                of<<"\n";
            }
        }
        for(int i=0; i<tp.size(); i++)
            of<<tp[i].first<<" "<<tp[i].second<<" "<<endl;
    }
    void draw() {
        for(Group g : group)
        {
            if(material[g.MaterialId].textured)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,material[g.MaterialId].tex);
                
            }else{
                glColor3f(material[g.MaterialId].color.x,material[g.MaterialId].color.y,material[g.MaterialId].color.z);
            }
                glPushMatrix();
            for(int i=g.first_face; i<=g.last_face;i++) {
                glBegin(GL_POLYGON);
                for(int j=0; j<face[i].size(); j++)
                {
                    if(material[g.MaterialId].textured)
                        glTexCoord2f(tp[face[i][j].second].first,tp[face[i][j].second].second);
                    glVertex3f(p[face[i][j].first].x,p[face[i][j].first].y,p[face[i][j].first].z);
                }
                glEnd();
                glPopMatrix();
            }
            if(material[g.MaterialId].textured)
            {
                glDisable(GL_TEXTURE_2D);
            }
        }
    
    }
};