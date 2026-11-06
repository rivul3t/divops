/* Version 0.5 */
/* eslint-disable */
const variables = {
    adsList: [],
    adsData: [],
    scrollBound: false,
    resizeBound: false
};

function initStickyAds(ads) {
    variables.adsList = ads;
    document.addEventListener('DOMContentLoaded', () => {
        if (!variables.adsList?.length) return;

        getStickyAdsElements();
        hasMarketingConsent();
        observeOsanoElements();
    });
}

function hasMarketingConsent() {
    if (window.Osano?.cm?.marketing) {
        bindStickyAdsScroll();
        bindStickyAdsResize();
        observeAds();
    }
}

function observeOsanoElements() {

    const handelOsanoSaveBtn = () =>{
        const osanoSaveBtn = document.querySelector('.osano-cm-save');
        osanoSaveBtn?.addEventListener('click', hasMarketingConsent)
    }

    document.querySelector('.mange-cookies-btn')?.addEventListener('click', handelOsanoSaveBtn);
    mutationObserver(
        '.osano-cm-window',
        { childList: true, subtree: true },
        () => {
            document.querySelector('.osano-cm-accept-all')?.addEventListener('click', hasMarketingConsent);
            document.querySelector('.osano-cm-manage')?.addEventListener('click', handelOsanoSaveBtn);
        },
        5000
    );
}

function getStickyAdsElements() {
    variables.adsList.forEach(adConfig => {
        const adEl = document.querySelector(adConfig.adSelector);
        if (!adEl) return;

        const ad = {
            ...adConfig,
            adElement: adEl,
            isAdLoaded: false,
            placeholder: null
        };

        variables.adsData.push(ad);
        setStickyAdsData(variables.adsData.length - 1);
    });
}

function observeAds() {
    const hasResizeObserver = typeof ResizeObserver !== 'undefined';

    variables.adsData.forEach((ad, index) => {
        if (!ad.isAdLoaded) {
            mutationObserver(
                `${ad.adSelector} iframe`,
                { childList: true, subtree: true },
                () => {
                    ad.isAdLoaded = true;
                    handleStickyAdsScroll(index);
                }
            );
        }

        if (hasResizeObserver && !ad.resizeObserverBound) {
            new ResizeObserver(() => handleAdSizeChange(index)).observe(ad.adElement);
            ad.resizeObserverBound = true;
        }
    });
}

function handleAdSizeChange(index) {
    const ad = variables.adsData[index];
    if (!ad) return;

    const rect = ad.adElement.getBoundingClientRect();
    const newHeight = rect.height;
    const newWidth = rect.width;

    if (newHeight === ad.height && newWidth === ad.width) return;

    ad.height = newHeight;
    ad.width = newWidth;

    if (ad.placeholder) {
        ad.placeholder.style.minHeight = (newHeight + ad.marginTop + ad.marginBottom) + 'px';
        ad.placeholder.style.width = newWidth + 'px';
    }

    ad.boundingElTop = null;
    ad.boundingElBottom = null;

    handleStickyAdsScroll(index, true);
}

function bindStickyAdsResize() {
    if (variables.resizeBound) return;

    window.addEventListener('resize', onStickyAdsResize);
    variables.resizeBound = true;
}

function bindStickyAdsScroll() {
    if (variables.scrollBound) return;

    window.addEventListener(
        'scroll',
        () => {
            variables.adsData.forEach((_, index) => {
                handleStickyAdsScroll(index);
            });
        },
        { passive: true }
    );

    variables.scrollBound = true;
}

function onStickyAdsResize() {
    variables.adsData.forEach((ad, index) => {
        const rect = ad.adElement.getBoundingClientRect();
        ad.left = ad.placeholder
            ? ad.placeholder.getBoundingClientRect().left
            : rect.left;
        handleStickyAdsScroll(index, true);
    });
}

function mutationObserver(selector, config, callback, timeout = null) {
    const observer = new MutationObserver(() => {
        const el = document.querySelector(selector);
        if (el) {
            callback(el);
            observer.disconnect();
        }
    });

    observer.observe(document, config);

    if (timeout) {
        setTimeout(() => observer.disconnect(), timeout);
    }
}

function setStickyAdsData(index) {
    const ad = variables.adsData[index];
    const adEl = ad.adElement;
    const rect = adEl.getBoundingClientRect();
    const styles = window.getComputedStyle(adEl);
    const adWrapperEl = ad.adElement.closest('.pb-las');

    ad.originalTop = rect.top + window.scrollY;
    ad.height = rect.height;
    ad.width = rect.width;
    ad.left = rect.left;
    ad.marginTop = parseInt(styles.marginTop) || 0;
    ad.marginBottom = parseInt(styles.marginBottom) || 0;
    ad.adIsFixed = false;
    ad.adIsStopped = false;
    ad.boundingPlaceholder = false;

    if(adWrapperEl) {
        const adWrapperStyles = window.getComputedStyle(adWrapperEl);
        ad.wrapperEl = adWrapperEl;
    }

    updateStickyAdsData(index)
}

function updateStickyAdsData(index) {
    const ad = variables.adsData[index];
    let boundEl = document.querySelector(ad.boundingElementSelector);

    if(ad.wrapperEl){
        const adWrapperRect = ad.wrapperEl.getBoundingClientRect();
        ad.originalTop = adWrapperRect.top  + window.scrollY;
    }

    if (!boundEl) return;

    if (!ad.boundingPlaceholder && (boundEl.offsetParent === null || boundEl.getBoundingClientRect().height === 0)) {
        const boundPlaceholder = document.createElement('div');
        boundEl.before(boundPlaceholder);
        boundEl = boundPlaceholder;
        ad.boundingPlaceholder= true;
    }

    const bRect = boundEl.getBoundingClientRect();
    const threshold = 5;

    ad.boundingElement = boundEl;
    const isFirstCalc = ad.boundingElTop == null;

    if (isFirstCalc) {
        ad.stopPosition = calcStopPosition(ad, bRect);
        ad.boundingElTop = bRect.top;
        ad.boundingElBottom = bRect.bottom;
        return;
    }

    const topDiff = Math.abs(ad.boundingElTop - bRect.top);
    const bottomDiff = Math.abs(ad.boundingElBottom - bRect.bottom);

    if (topDiff > threshold || bottomDiff > threshold) {
        ad.stopPosition = calcStopPosition(ad, bRect);
        ad.boundingElTop = bRect.top;
        ad.boundingElBottom = bRect.bottom;
    }
}

function calcStopPosition(ad, bRect) {
    return ad.boundedByEdge === 'top'
        ? bRect.top + window.scrollY - ad.height
        : bRect.bottom + window.scrollY - ad.height;
}

function createAdPlaceholder(ad, height, width) {
    const placeholder = document.createElement('div');
    placeholder.className = 'sticky-ad-placeholder';
    placeholder.style.minHeight = height + 'px';
    placeholder.style.width = width + 'px';

    ad.adElement.before(placeholder);
    ad.placeholder = placeholder;
}

const STICKY_RIGHT_RAIL_MIN_WIDTH = 992;

function handleStickyAdsScroll(index, resize = false) {
    const scrollY = window.scrollY;
    const ad = variables.adsData[index];

    if (!ad) return;

    const {
        stopPosition,
        adIsFixed,
        adIsStopped,
        left,
        height,
        width,
        marginTop,
        marginBottom,
        isAdLoaded,
        adElement,
        boundingElement
    } = ad;

    const isRightRailAd = !!adElement.closest('.col-md-4');

    // Right rail collapses below this width, so its ad shouldn't stick
    if (isRightRailAd && window.innerWidth < STICKY_RIGHT_RAIL_MIN_WIDTH) {
        if (adIsFixed || adIsStopped) {
            adElement.classList.remove('sticky-ad--fixed', 'sticky-ad--stopped');

            if (ad.placeholder) {
                ad.placeholder.remove();
                ad.placeholder = null;
            }

            adElement.style.position = '';
            adElement.style.left = '';
            adElement.style.top = '';

            ad.adIsFixed = false;
            ad.adIsStopped = false;
        }
        return;
    }

    const adMargin = marginTop + marginBottom;
    updateStickyAdsData(index)


    // Reset
    if (scrollY < ad.originalTop) {
        if (adIsFixed || adIsStopped) {
            adElement.classList.remove('sticky-ad--fixed', 'sticky-ad--stopped');

            if (ad.placeholder) {
                ad.placeholder.remove();
                ad.placeholder = null;
            }

            adElement.style.position = '';
            adElement.style.left = '';
            adElement.style.top = '';

            ad.adIsFixed = false;
            ad.adIsStopped = false;
        }
        return;
    }

    // Stop at boundary
    if (ad.stopPosition && scrollY >= ad.stopPosition && boundingElement ) {
        if (!ad.placeholder && isAdLoaded) {
            createAdPlaceholder(ad, height + adMargin, width);
        }

        adElement.classList.remove('sticky-ad--fixed');
        adElement.classList.add('sticky-ad--stopped');
        adElement.style.position = 'absolute';

        if (adElement.closest('.col-md-4')) {
            const rightRail = document.querySelector('main .col-md-12 .col-md-4');
            const rightRailRect = rightRail.getBoundingClientRect();
            const rightRailTop = rightRailRect.top + scrollY;

            adElement.style.top = stopPosition - rightRailTop - marginTop + 'px';
            adElement.style.left = ''
        } else {
            adElement.style.top = stopPosition + 'px';
        }

        ad.adIsStopped = true;
        ad.adIsFixed = false;

        return;
    }

    // Sticky mode
    if (isAdLoaded && (!adIsFixed || resize) && scrollY >= ad.originalTop) {
        if (!ad.placeholder) {
            createAdPlaceholder(ad, height + adMargin, width);
        }

        adElement.classList.add('sticky-ad--fixed');
        adElement.classList.remove('sticky-ad--stopped');

        adElement.style.position = '';
        adElement.style.left = left + 'px';
        adElement.style.top = -marginTop + 'px';

        ad.adIsFixed = true;
        ad.adIsStopped = false;
    }
}


window.StickyAds = {
    init: initStickyAds
};
/* eslint-disable */