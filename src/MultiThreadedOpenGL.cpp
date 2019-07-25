/**

File: MultiThreadedOpenGL.cpp
Description:

Handles all openGL calls and requests to the correct thread

THREADING IN OPERATION, might be safe

*/

#include "MultiThreadedOpenGL.h"

using namespace darksun;

void mtopengl::process() {
	// Process VAO allocations
	processVAOLoadRequests();
	// Process texture loads
	processTextureLoadRequests();
	// Process VBO updates
	processVBOUpdateRequests();
}

/**

sf::Event handling

*/

static std::mutex eventQueue_mutex = std::mutex();
static std::vector<sf::Event> eventQueue = std::vector<sf::Event>();

std::vector<sf::Event> mtopengl::getEvents() {
	std::lock_guard lock(eventQueue_mutex);
	std::vector<sf::Event> retEvents = eventQueue;
	eventQueue.clear();
	return retEvents;
}

void mtopengl::addEvent(sf::Event e) {
	std::lock_guard lock(eventQueue_mutex);
	eventQueue.push_back(e);
	//dout.verbose("Detected event " + std::to_string(e.type) + " with queue length (post-addition) = " + std::to_string(eventQueue.size()));
}

/**

VAO Loading

*/
static std::mutex loadingVAOs_mutex = std::mutex();

static std::vector<mtopengl::VAODef> vaosToLoad = std::vector<mtopengl::VAODef>();

static std::map<int, mtopengl::VAODef> loadedVAOs = std::map<int, mtopengl::VAODef>();

static int VAO_REF_COUNTER = 0;

unsigned int mtopengl::getVAO(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices) {
	int ref = ++VAO_REF_COUNTER;
	
	unsigned int vaoId = 0;
	int countLoaded = 0;
	// Get the number of vaos loaded with this filename
	{
		std::lock_guard lock(loadingVAOs_mutex);
		countLoaded = loadedVAOs.count(ref);
	}

	if (countLoaded == 0) {
		dout.log("OpenGL --> Got request for VAO with ref \"" + std::to_string(ref) + "\" which is not yet loaded, loading now");

		// There are none loaded, schedule one for loading
		mtopengl::VAODef def;
		def.ref = ref;
		def.vertices = std::vector<Vertex>(*vertices);
		def.indices = std::vector<unsigned int>(*indices);
		// add to the loading vector
		{
			std::lock_guard lock(loadingVAOs_mutex);
			vaosToLoad.push_back(def);
		}

		// Now we wait until the loaded count goes above 0
		while (countLoaded == 0) {
			{
				std::lock_guard lock(loadingVAOs_mutex);
				countLoaded = loadedVAOs.count(ref);
			}
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
	}

	// It is loaded, set the return value and return
	{
		std::lock_guard lock(loadingVAOs_mutex);
		vaoId = loadedVAOs[ref].VAO;
	}

	// Do a check on the return value
	if (vaoId == 0) {
		// NULL!
		dout.error("getVAO() is about to return a null pointer for VAO ref \"" + std::to_string(ref) + "\"");
	}

	return vaoId;
}

void mtopengl::processVAOLoadRequests() {
	profiler::ScopeProfiler profiler("MultiThreadedOpenGL.cpp::mtopengl::processVAOLoadRequests()");
	// Acquire the locks for the duration of the processing
	std::lock_guard lock(loadingVAOs_mutex);

	for (auto const& e : vaosToLoad) {
		mtopengl::VAODef def = e;
		// Do the load
		
		glGenVertexArrays(1, &def.VAO);
		glGenBuffers(1, &def.VBO);
		glGenBuffers(1, &def.EBO);

		glBindVertexArray(def.VAO);

		def.VBOSize = def.vertices.size() * sizeof(Vertex);
		glBindBuffer(GL_ARRAY_BUFFER, def.VBO);
		glBufferData(GL_ARRAY_BUFFER, def.VBOSize, &def.vertices[0], GL_STATIC_DRAW);

		def.EBOSize = def.indices.size() * sizeof(unsigned int);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, def.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, def.EBOSize, &def.indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);

		// CLEAR THE VECTORS
		def.vertices.clear();
		def.indices.clear();
		// Put the def in the loadedVAOs map
		loadedVAOs[def.ref] = def;
	}

	// clear the toLoad pile
	vaosToLoad.clear();
}

/**

VAO Updating

*/
// We use the loadingVAOs mutex for these functions

static std::vector<mtopengl::VBOUpdateDef> vbosToUpdate = std::vector<mtopengl::VBOUpdateDef>();

void mtopengl::updateVBO(int vaoRef, std::vector<Vertex>* vertices) {
	profiler::ScopeProfiler profiler("MultiThreadedOpenGL.cpp::mtopengl::updateVBO()");
	// Acquire the locks for the duration of the processing
	std::lock_guard lock(loadingVAOs_mutex);

	// Check to see if the VAO exists:
	if(loadedVAOs.count(vaoRef) <= 0) {
		dout.error("MultiThreadedOpenGL.cpp::mtopengl::updateVBO() --> Attempted to add VBO update of VAORef='" + std::to_string(vaoRef) + "' where such VAORef doesn't exist");
		return;
	}
	
	VBOUpdateDef def;
	def.vaoDefRef = vaoRef;
	def.vertices = std::vector<Vertex>(*vertices);

	vbosToUpdate.push_back(def);
}

void mtopengl::processVBOUpdateRequests() {
	profiler::ScopeProfiler profiler("MultiThreadedOpenGL.cpp::mtopengl::processVBOUpdateRequests()");
	// Acquire the locks for the duration of the processing
	std::lock_guard lock(loadingVAOs_mutex);

	for (auto& def : vbosToUpdate) {
		// VAOref is assumed to exist, otherwise it wouldn't have made it into the list
		glBindVertexArray(loadedVAOs[def.vaoDefRef].VAO);

		// bind the VBO we want to update
		glBindBuffer(GL_ARRAY_BUFFER, loadedVAOs[def.vaoDefRef].VBO);

		// Do the data swap
		glBufferSubData(GL_ARRAY_BUFFER, 0, loadedVAOs[def.vaoDefRef].VBOSize, &def.vertices[0]);

		// Unbind
		glBindVertexArray(0);
	}

	vbosToUpdate.clear();
}


/**

Texture Loading

*/
static std::mutex loadingTextures_mutex = std::mutex();

static std::vector<mtopengl::TextureDef> texturesToLoad = std::vector<mtopengl::TextureDef>();

static std::map<string, mtopengl::TextureDef> loadedTextures = std::map<string, mtopengl::TextureDef>();

unsigned int mtopengl::getTexture(const string filename, bool gamma) {
	unsigned int textId = 0;
	int countLoaded = 0;
	// Get the number of textures loaded with this filename
	{
		std::lock_guard lock(loadingTextures_mutex);
		countLoaded = loadedTextures.count(filename);
	}

	if (countLoaded == 0) {
		dout.log("OpenGL --> Got request for texture \"" + filename + "\" which is not yet loaded, loading now");

		// There are none loaded, schedule one for loading
		mtopengl::TextureDef def;
		def.filename = filename;
		def.gamma = gamma;
		// add to the loading vector
		{
			std::lock_guard lock(loadingTextures_mutex);
			texturesToLoad.push_back(def);
		}

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);

		// Now we wait until the loaded count goes above 0
		dout.verbose("OpenGL --> Waiting for the texture....");
		while (countLoaded == 0) {
			{
				std::lock_guard lock(loadingTextures_mutex);
				countLoaded = loadedTextures.count(filename);
			}
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
		dout.verbose("OpenGL --> Got the texture");
	}

	// It is loaded, set the return value and return
	{
		std::lock_guard lock(loadingTextures_mutex);
		textId = loadedTextures[filename].id;
	}

	// Do a check on the return value
	if (textId == 0) {
		// NULL!
		dout.error("getTexture() is about to return a null pointer for texture filename \"" + filename + "\"");
	}

	return textId;
}

void mtopengl::processTextureLoadRequests() {
	profiler::ScopeProfiler profiler("MultiThreadedOpenGL.cpp::mtopengl::processTextureLoadRequests()");
	// Acquire the locks for the duration of the processing
	std::lock_guard lock(loadingTextures_mutex);

	for (auto const& e : texturesToLoad) {
		mtopengl::TextureDef def = e;
		// Do the load
		def.id = textureFromFile(def.filename, def.gamma);
		// Put the def in the loadedTextures map
		loadedTextures[def.filename] = def;
	}

	// clear the toLoad pile
	texturesToLoad.clear();
}

unsigned int mtopengl::textureFromFile(const string filename, bool gamma) {

	unsigned int textureID = 0;

	if (!std::filesystem::exists(filename)) {
		dout.error("Could not load texture: does not exist! (" + filename + ")");
		return textureID;
	}
	dout.log("textureFromFile('" + filename + "', '" + std::to_string(gamma) + "')");

	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		dout.error("Texture failed to load at path: " + filename);
		stbi_image_free(data);
	}

	return textureID;
}