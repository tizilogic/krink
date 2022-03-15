let project = new Project('DragDrop');

await project.addProject('../../');

project.addFile('Sources/**');
project.setDebugDir('Deployment');
project.addDefine('KINC_NO_WAYLAND');

project.flatten();

resolve(project);
