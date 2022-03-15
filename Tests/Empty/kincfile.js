let project = new Project('Empty');

await project.addProject('../..');
project.addFile('Sources/**');
project.setDebugDir('Deployment');
project.addDefine('KINC_NO_WAYLAND');

resolve(project);
