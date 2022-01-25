let project = new Project('krink');

await project.addProject('Kinc');

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.addIncludeDir('Sources');

project.flatten();

resolve(project);
