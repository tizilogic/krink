let project = new Project('DragDrop');

await project.addProject('../..');
project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
