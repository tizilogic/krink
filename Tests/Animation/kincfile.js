let project = new Project('Animation');

await project.addProject('../..');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
