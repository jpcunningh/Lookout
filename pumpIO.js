const Debug = require('debug');
/*eslint-disable-next-line no-unused-vars*/
const log = Debug('pumpio:log');
const error = Debug('pumpio:error');
const debug = Debug('pumpio:debug');

const chokidar = require('chokidar');
const fs = require('fs');

module.exports = (io, options) => {
  let basalProfile;
  const openapsDir = options.openaps;

  const readBasalProfile = (path) => {
    debug(`Reading file ${path}`);
    setTimeout(function() {
      fs.readFile(path, 'utf8', function (err, data) {
        if (err) {
          error('Error reading file: ' + path);
          return; // we'll not consider error handling for now
        }
        try {
          basalProfile = JSON.parse(data);
          io.emit('basalProfile', basalProfile);
        } catch (e) {
          error('Error parsing JSON file: ' + path);
          return;
        }
      });
    }, 1000);
  };

  chokidar.watch(openapsDir + '/settings/basal_profile.json')
    .on('change', readBasalProfile)
    .on('add', readBasalProfile);

  io.on('connection', socket => {
    socket.emit('basalProfile', basalProfile);
    // iob = require('/root/myopenaps/monitor/iob.json');
    // socket.emit('iob', iob[0]['iob']);
  });
};
