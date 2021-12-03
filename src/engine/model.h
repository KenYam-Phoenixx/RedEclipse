enum { MDL_MD2 = 0, MDL_MD3, MDL_MD5, MDL_OBJ, MDL_SMD, MDL_IQM, NUMMODELTYPES };
enum { MDL_BLEND_TEST = 0, MDL_BLEND_ALPHA };

struct parenttag
{
    char *name;
    matrix4x3 matrix;

    parenttag() : name(NULL) {}
    ~parenttag() { DELETEA(name); }
};

struct model
{
    char *name;
    float spinyaw, spinpitch, spinroll, offsetyaw, offsetpitch, offsetroll;
    bool shadow, alphashadow, depthoffset;
    float wind, scale;
    vec translate;
    BIH *bih;
    vec bbcenter, bbradius, bbextend, collidecenter, collideradius;
    float rejectradius, height, collidexyradius, collideheight;
    char *collidemodel;
    int collide, batch;
    vector<parenttag> parenttags;

    model(const char *name) : name(name ? newstring(name) : NULL), spinyaw(0), spinpitch(0), spinroll(0), offsetyaw(0), offsetpitch(0), offsetroll(0), shadow(true), alphashadow(true), depthoffset(false), wind(0.0f), scale(1.0f), translate(0, 0, 0), bih(0), bbcenter(0, 0, 0), bbradius(-1, -1, -1), bbextend(0, 0, 0), collidecenter(0, 0, 0), collideradius(-1, -1, -1), rejectradius(-1), height(0.9f), collidexyradius(0), collideheight(0), collidemodel(NULL), collide(COLLIDE_OBB), batch(-1) {}
    virtual ~model() { DELETEA(name); DELETEP(bih); }
    virtual void calcbb(vec &center, vec &radius) = 0;
    virtual void calctransform(matrix4x3 &m) = 0;
    virtual int intersect(int anim, modelstate *state, dynent *d, const vec &o, const vec &ray, float &dist, int mode = 0) = 0;
    virtual void render(int anim, modelstate *state, dynent *d = NULL) = 0;
    virtual bool load() = 0;
    virtual int type() const = 0;
    virtual BIH *setBIH() { return NULL; }
    virtual bool envmapped() const { return false; }
    virtual bool skeletal() const { return false; }
    virtual bool animated() const { return false; }
    virtual bool pitched() const { return true; }
    virtual bool alphatested() const { return false; }
    virtual bool alphablended() const { return false; }
    virtual bool needscolor() const { return wind != 0.0f; }

    virtual void setshader(Shader *shader) {}
    virtual void setenvmap(float envmapmin, float envmapmax, Texture *envmap) {}
    virtual void setspec(float spec) {}
    virtual void setgloss(int gloss) {}
    virtual void setglow(float glow, float glowdelta, float glowpulse) {}
    virtual void setalphatest(float alpha) {}
    virtual void setdither(bool dither) {}
    virtual void setblend(float blend) {}
    virtual void setblendmode(int mode) {}
    virtual void setfullbright(float fullbright) {}
    virtual void setcullface(int cullface) {}
    virtual void setcullhalo(bool cullhalo) {}
    virtual void setcolor(const vec &color) {}
    virtual void setmaterial(int material1, int material2) {}
    virtual void setmixer(bool) {}
    virtual void setpattern(bool) {}

    virtual void genshadowmesh(vector<triangle> &tris, const matrix4x3 &orient) {}
    virtual void preloadBIH() { if(!bih) setBIH(); }
    virtual void preloadshaders() {}
    virtual void preloadmeshes() {}
    virtual void cleanup() {}

    virtual void startrender() {}
    virtual void endrender() {}

    void boundbox(vec &center, vec &radius)
    {
        if(bbradius.x < 0)
        {
            calcbb(bbcenter, bbradius);
            bbradius.add(bbextend);
        }
        center = bbcenter;
        radius = bbradius;
    }

    float collisionbox(vec &center, vec &radius)
    {
        if(collideradius.x < 0)
        {
            boundbox(collidecenter, collideradius);
            if(collidexyradius)
            {
                collidecenter.x = collidecenter.y = 0;
                collideradius.x = collideradius.y = collidexyradius;
            }
            if(collideheight)
            {
                collidecenter.z = collideradius.z = collideheight/2;
            }
            rejectradius = vec(collidecenter).abs().add(collideradius).magnitude();
        }
        center = collidecenter;
        radius = collideradius;
        return rejectradius;
    }

    float boundsphere(vec &center)
    {
        vec radius;
        boundbox(center, radius);
        return radius.magnitude();
    }

    float above()
    {
        vec center, radius;
        boundbox(center, radius);
        return center.z+radius.z;
    }

    virtual void addlod(const char *str, float dist) {}
    virtual const char *lodmodel(float sqdist = 0, float sqoff = 0) { return NULL; }
};

