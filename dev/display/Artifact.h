#ifndef ARTIFACT_H
#define ARTIFACT_H

class Screen;

class Artifact {
public:
	// Destructor
	virtual ~Artifact() { return; }

	// Rendering function
	virtual void render(Screen * curscr) = 0;
};

#endif
