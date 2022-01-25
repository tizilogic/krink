let project = new Project('Empty');

await project.addProject('../../Kinc');
await project.addProject('../../');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
