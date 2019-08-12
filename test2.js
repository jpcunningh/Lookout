const Debug = require('debug');

const log = Debug('transmitterIO:log');
const error = Debug('transmitterIO:error');
/* eslint-disable-next-line no-unused-vars */
const debug = Debug('transmitterIO:debug');

const moment = require('moment');
const persist = require('node-persist');
const store = require('node-persist');
const storage = require('./storage');
const xDripAPS = require('./xDripAPS')();
const syncNS = require('./syncNS');
const TimeLimitedPromise = require('./timeLimitedPromise');
const calibration = require('./calibration');

/* eslint-disable-next-line no-unused-vars */
const test1 = async () => {
  let body = await xDripAPS.latestSGVs(10)
    .catch((err) => {
      log(`Error getting latest SGVs from NS: ${err}`);
    });

  if (body && (body.length > 0)) {
    log(`Found ${body.length} SGV entries\n%O`, body);
  }

  let timeSince = moment().subtract(2, 'hours');

  log(`Querying SGVs since ${timeSince.toISOString()}`);
  body = await xDripAPS.SGVsSince(timeSince, 12 * 24 * 2)
    .catch((err) => {
      error(`Error getting latest SGVs Since Time from NS: ${err}`);
    });

  if (body && (body.length > 0)) {
    log(`Found ${body.length} SGV entries since ${timeSince.toISOString()}\n%O`, body);
  }

  timeSince = moment('2018-12-02T00:15:00-05:00');

  log(`Querying BG Checks since ${timeSince.format()}`);
  body = await xDripAPS.BGChecksSince(timeSince)
    .catch((err) => {
      log(`Error getting BG Checks Since Time from NS: ${err}`);
    });

  if (body && (body.length > 0)) {
    log(`Found ${body.length} BG Checks since ${timeSince.format()}\n%O`, body);
  }

  const calEntry = await xDripAPS.latestCal()
    .catch((err) => {
      log(`Error getting latest cal from NS: ${err}`);
    });

  if (calEntry) {
    log('Found calibration entry\n%O', calEntry);
  }

  const insertTime = await xDripAPS.latestEvent('Sensor Change')
    .catch((err) => {
      error(`Error getting latest sensor insert from NS: ${err}`);
    });

  if (insertTime) {
    log('Found sensor insert\n%O', insertTime.format());
  }
};

/* eslint-disable-next-line no-unused-vars */
const test3 = async () => {
  /* eslint-disable-next-line no-unused-vars */
  const p = new TimeLimitedPromise(1000, ((resolve, reject) => {

    // We never resolve/reject, so we test the timeout
  }));

  p.then((result) => {
    log(`Resolved: ${result}`);
  }).catch((reject) => {
    log(`Rejected: ${reject}`);
  });
};

/* eslint-disable-next-line no-unused-vars */
const test4 = async () => {
  await store.init({ dir: `${__dirname}/storage` });
  await storage.init(store);

  await syncNS({ }, storage, null);

  const lastCal = await storage.getItem('g5Calibration')
    .catch((err) => {
      error(`Unable to obtain current NS Calibration${err}`);
    });

  const expiredCal = await storage.getItem('expiredCal')
    .catch((err) => {
      error(`Unable to obtain current expired Calibration${err}`);
    });

  const glucoseHist = await storage.getItem('glucoseHist')
    .catch((err) => {
      error(`Error getting glucoseHist: ${err}`);
    });

  if (glucoseHist && glucoseHist.length > 0 && lastCal) {
    log(`     Txmitter Cal Value: ${calibration.calcGlucose(glucoseHist[glucoseHist.length - 1], lastCal)}`);
  } else {
    log('No Txmitter Cal Value');
  }

  if (glucoseHist && glucoseHist.length > 0 && expiredCal) {
    log(`Expired Cal Value: ${calibration.calcGlucose(glucoseHist[glucoseHist.length - 1], expiredCal)}`);
  } else {
    log('No Expired Cal Value');
  }
};

/* eslint-disable-next-line no-unused-vars */
const test5 = async () => {
  let NSCal = null;

  NSCal = await xDripAPS.latestCal()
    .catch((err) => {
      error(`Error getting NS calibration: ${err}`);
    });

  log(moment(NSCal.date).format());
  log(NSCal);
};

/* eslint-disable-next-line no-unused-vars */
const test6 = async () => {
  await persist.init({ dir: `${__dirname}/storage`, forgiveParseErrors: true });

  storage.init(persist);

  const item = await storage.getItem('test1'); /* eslint-disable-line no-unused-vars */

  console.log(item);

  storage.setItem('test1', 'test string');
};

let lookoutDebug = 'calcStats:*,calibration:*,clientIO:*,fakemeter:*,loopIO:*';
lookoutDebug += ',pumpIO:*,storageLock:*,syncNS:*,transmitterIO:*,transmitterWorker:*';
lookoutDebug += ',xDripAPS:*,transmitter';

Debug.enable(lookoutDebug);

// test1();
// test3();
test4();
// test5();
// test6();
