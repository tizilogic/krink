let project = new Project('krink');

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.addIncludeDir('Sources');

resolve(project);
