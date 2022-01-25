let project = new Project('Empty');

await project.addProject('../..');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
