
package cn.edu.zju.acm.mvc.control.annotation;

import cn.edu.zju.acm.mvc.control.annotation.OneException;

@OneException(exception = Exception.class, result = "jsp")
public class TestSingleExceptionAction extends TestExceptionActionBase {
}
